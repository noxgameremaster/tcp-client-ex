
#include "eventworker.h"
#include "loopThread.h"

EventWorker::EventWorker()
    : CCObject()
{
    m_eventThread = std::make_unique<LoopThread>();
    m_eventThread->SetTaskFunction([this]() { return this->CheckoutEvent(); });
    m_eventThread->SetWaitCondition([this]() { return this->IsTask(); });
}

EventWorker::~EventWorker()
{ }

bool EventWorker::IsTask() const
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        return !m_task.empty();
    }
}

bool EventWorker::CheckoutEvent()
{
    std::function<void()> task;

    {
        std::lock_guard<std::mutex> guard(m_lock);

        task = m_task.front();
        m_task.pop_front();
    }
    if (task)
        task();
    return true;
}

bool EventWorker::Start()
{
    return m_eventThread->Startup();
}

bool EventWorker::Stop()
{
    m_eventThread->Shutdown();
    return true;
}

EventWorker &EventWorker::Instance()
{
    static EventWorker worker;

    return worker;
}

void EventWorker::EventThreadNotify()
{
    if (m_eventThread)
        m_eventThread->Notify();
}