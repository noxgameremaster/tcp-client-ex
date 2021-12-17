
#ifndef EVENT_WORKER_H__
#define EVENT_WORKER_H__

#include "ccobject.h"

class LoopThread;

class EventWorker : public CCObject
{
private:
    std::list<std::function<void()>> m_task;
    std::unique_ptr<LoopThread> m_eventThread;

private:
    explicit EventWorker();

public:
    EventWorker(const EventWorker &) = delete;
    EventWorker(EventWorker &&) = delete;
    EventWorker &operator=(const EventWorker &) = delete;
    EventWorker &operator=(EventWorker &&) = delete;

    ~EventWorker() override;

private:
    bool IsTask() const;
    void CheckoutEvent();
    void Work();

public:

    template <class... SignalArgs, class... Args>
    void AppendTask(EventSignal<SignalArgs...> *signal, Args&&... args)
    {
        if (signal == nullptr)
            return;

        CCObject *cobject = dynamic_cast<CCObject *>(signal);

        if (cobject == nullptr)
            return;

        {
            std::lock_guard<std::mutex> guard(m_lock);
            m_task.push_back([ref = std::weak_ptr<CCObject::Pimpl>((*cobject)()), args..., signal]()
            {
                if (ref.expired())
                    return;
                signal->Emit(args...);
            }
            );
            EventThreadNotify();
        }
    }

    bool Start();
    bool Stop();
    void EventThreadNotify();

    static EventWorker &Instance();

private:
    mutable std::mutex m_lock;
};

#endif

