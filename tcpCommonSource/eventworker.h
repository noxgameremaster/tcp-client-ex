
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
    bool CheckoutEvent();

public:

    template <class... SignalArgs, class... Args>
    void AppendTask(EventSignal<SignalArgs...> *signal, Args&&... args)
    {
        if (signal == nullptr)
            return;

        CCObject *cobject = dynamic_cast<CCObject *>(signal);

        if (cobject == nullptr)
            return;

        std::weak_ptr<CCObject::Pimpl> observer((*cobject)());

        {
            std::lock_guard<std::mutex> guard(m_lock);

            if (observer.expired())
                return;

            m_task.push_back([ref = observer, args..., signal]()
            {
                if (ref.expired())
                    return;
                signal->Emit(args...);
            }
            );
            ////EventThreadNotify();
        }
        EventThreadNotify();
    }

    bool Start();
    bool Stop();
    void EventThreadNotify();

    static EventWorker &Instance();

private:
    mutable std::mutex m_lock;
};

#define QUEUE_EMIT(SignalObject, ...)   \
    EventWorker::Instance().AppendTask(&SignalObject, __VA_ARGS__);

#endif

