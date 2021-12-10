
#include "eventworker.h"

EventWorker::EventWorker()
{
    m_terminate = false;
}

EventWorker::~EventWorker()
{ }

void EventWorker::Work()
{
    while (true)
    {
        std::function<void()> task = nullptr;

        {
            std::unique_lock<std::mutex> lock(m_lock);
            m_condvar.wait(lock, [this]() { return !m_task.empty() || m_terminate; });

            if (m_terminate)
                break;

            task = m_task.front();
            m_task.pop_front();
        }
        if (task)
            task();
    }
}

bool EventWorker::Start()
{
    if (m_worker.joinable())
        return false;

    m_worker = std::thread([this]() { this->Work(); });
    return true;
}

bool EventWorker::Stop()
{
    if (!m_worker.joinable())
        return false;

    m_terminate = true;
    m_condvar.notify_all();
    m_worker.join();
    return true;
}

EventWorker &EventWorker::Instance()
{
    static EventWorker worker;

    return worker;
}
