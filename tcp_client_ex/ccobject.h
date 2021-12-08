
#ifndef EVENT_SIGNAL_H__
#define EVENT_SIGNAL_H__

#include <functional>
#include <list>
#include <memory>
#include <mutex>

class CCObject
{
protected:
    class Pimpl
    {
        CCObject *m_parent;

    public:
        explicit Pimpl(CCObject *parent)
            : m_parent(parent)
        { }

        CCObject *operator()()
        {
            return m_parent;
        }
    };

    std::shared_ptr<Pimpl> m_pimpl;

protected:
    explicit CCObject()
        : m_pimpl(new Pimpl(this))
    {
    }

    virtual ~CCObject()
    { }

public:
    std::shared_ptr<Pimpl> operator()()
    {
        return m_pimpl;
    }
};

namespace _event_signal_slot_private
{
    template <class... Args>
    class EventSlot;
}

template <class... Args>
class _event_signal_slot_private::EventSlot : public CCObject
{
    using slot_functionType = std::function<void(Args...)>;
private:
    slot_functionType m_slot;
    std::weak_ptr<CCObject::Pimpl> m_refTarget;

public:
    explicit EventSlot(slot_functionType &&slot, std::shared_ptr<CCObject::Pimpl> target)
        : CCObject(),
        m_slot(std::forward<slot_functionType>(slot))
    {
        m_refTarget = target;
    }

    ~EventSlot() override
    { }

    template <class... SlotArgs>
    void Emit(SlotArgs&&... args)
    {
        m_slot(std::forward<SlotArgs>(args)...);
    }

    inline bool Expired() const
    {
        return m_refTarget.expired();
    }
};

template <class... Args>
class EventSignal : public CCObject
{
    using slot_type = _event_signal_slot_private::EventSlot<Args...>;
    using slot_container = std::list<std::shared_ptr<slot_type>>;
    using slot_iterator = typename slot_container::iterator;

private:
    slot_container m_slots;

public:
    explicit EventSignal()
        : CCObject()
    { }

    ~EventSignal() override
    { }

private:
    template <class SlotFunction>
    void addSlot(SlotFunction &&slot, CCObject *pRecv)
    {
        auto slotObject = std::shared_ptr<slot_type>(new slot_type(std::forward<SlotFunction>(slot), (*pRecv)()));

        m_slots.push_back(std::move(slotObject));
    }

public:
    template <class SlotFunction, class RecvInstance>
    bool Connection(SlotFunction &&slot, RecvInstance *pRecv)
    {
        static_assert(std::is_base_of<CCObject, RecvInstance>::value, "the instance must inherit CCObject");
        if (pRecv == nullptr)
            return false;

        std::weak_ptr<CCObject::Pimpl> refptr = (*static_cast<CCObject *>(pRecv))();
        std::lock_guard<std::mutex> lock(m_lock);
        addSlot([callable = std::move(slot), refpt = std::move(refptr)](Args&&... args)
        {
            if (refpt.expired())
                return;

            std::shared_ptr<CCObject::Pimpl> recv = refpt.lock();
            (static_cast<RecvInstance *>((*recv)())->*callable)(std::forward<Args>(args)...);
        }, pRecv);
        return true;
    }

    template <class... SlotArgs>
    void Emit(SlotArgs&&... args)
    {
        slot_iterator travsCur = m_slots.begin();
        std::list<slot_iterator> removeSlotList;

        while (travsCur != m_slots.end())
        {
            if (!(*travsCur)->Expired())
                (*travsCur)->Emit(std::forward<SlotArgs>(args)...);
            else
                removeSlotList.push_back(travsCur);
            ++travsCur;
        }

        std::lock_guard<std::mutex> lock(m_lock);
        for (const auto &iter : removeSlotList)
            m_slots.erase(iter);
    }

private:
    std::mutex m_lock;
};

#define DECLARE_SIGNAL(SIGNAL_IDENTIFIER, ...)	\
private: \
    EventSignal<__VA_ARGS__> m_##SIGNAL_IDENTIFIER; \
public: \
    EventSignal<__VA_ARGS__> &SIGNAL_IDENTIFIER()   \
    { return m_##SIGNAL_IDENTIFIER; }    \

#endif
