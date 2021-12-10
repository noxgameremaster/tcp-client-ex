
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

