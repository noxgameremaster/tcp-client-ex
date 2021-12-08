
#ifndef EVENT_WORKER_H__
#define EVENT_WORKER_H__

#include "ccobject.h"

class EventWorker : public CCObject
{
private:
    std::list<std::function<void()>> m_task;
    std::condition_variable m_condvar;
    std::thread m_worker;
    bool m_terminate;

private:
    explicit EventWorker();

public:
    EventWorker(const EventWorker &) = delete;
    EventWorker(EventWorker &&) = delete;
    EventWorker &operator=(const EventWorker &) = delete;
    EventWorker &operator=(EventWorker &&) = delete;

    ~EventWorker() override;

private:
    void Work();

public:
    template <class... Args>
    void AppendTask(CCObject *signalobject, Args... args)
    {
        auto signal = static_cast<EventSignal<Args...>*>(signalobject);

        if (signal == nullptr)
            return;

        {
            std::lock_guard<std::mutex> localLock(m_lock);
            m_task.push_back([ref = std::weak_ptr<CCObject::Pimpl>((*signalobject)()), args..., signal]()
            {
                if (ref.expired())
                    return;
                signal->Emit(args...);  //FIXME//
            });
            m_condvar.notify_one();
        }
    }

    bool Start();
    bool Stop();

    static EventWorker &Instance();

private:
    std::mutex m_lock;
};

#endif
