
#include "eventworker.h"

EventWorker::EventWorker()
    : CCObject()
{
    m_eventThread = std::async(std::launch::async, [this]() { return CheckoutEvent(); });
}

EventWorker::~EventWorker()
{
    m_terminated = true;
    m_condvar.notify_all();

    m_eventThread.get();
}

bool EventWorker::IsTask() const
{
    return !m_task.empty();
}

bool EventWorker::CheckoutEvent()
{
    std::function<void()> task;

    for (;;)
    {
        {
            std::unique_lock<std::mutex> ulock(m_lock);

            m_condvar.wait(ulock, [this]() { return m_terminated || !m_task.empty(); });

            if (m_terminated)
                break;

            task = m_task.front();
            m_task.pop_front();
        }
        task();
    }
    return true;
}

EventWorker &EventWorker::Instance()
{
    static EventWorker worker;

    return worker;
}

void EventWorker::EventThreadNotify()
{
    m_condvar.notify_one();
}