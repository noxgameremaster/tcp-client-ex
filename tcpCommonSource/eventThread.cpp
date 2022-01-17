
#include "eventThread.h"

EventThread::EventThread(NetObject *parent)
    : NetService(parent)
{
    m_condition = [this]() { return false; };
    m_execution = [this]() { return false; };
}

EventThread::~EventThread()
{
    Shutdown();
}

void EventThread::OnInitialOnce()
{ }

bool EventThread::OnInitialize()
{
    if (m_lock.expired())
        return false;

    m_asyncTask = std::async(std::launch::async, &EventThread::Running, this);
    return true;
}

bool EventThread::OnStarted()
{
    return true;
}

void EventThread::OnDeinitialize()
{
    if (m_asyncTask.valid())
    {
        m_terminated = true;
        m_condvar.notify_all();
        m_asyncTask.get();
    }
}

void EventThread::OnStopped()
{ }

bool EventThread::Running()
{
    do
    {
        {
            auto getlock = m_lock.expired() ? nullptr : m_lock.lock();

            {
                std::unique_lock<std::mutex> lock(*getlock);

                m_condvar.wait(lock, [this]() { return m_terminated || m_condition(); });
                if (m_terminated)
                    break;
            }
        }
        if (!m_execution())
            return false;
    }
    while (true);

    return true;
}

void EventThread::SetCondition(std::function<bool()> &&condition)
{
    m_condition = std::forward<std::remove_reference<decltype(condition)>::type>(condition);
}

void EventThread::SetExecution(std::function<bool()> &&execution)
{
    m_execution = std::forward<std::remove_reference<decltype(execution)>::type>(execution);
}

void EventThread::Notify()
{
    m_condvar.notify_one();
}

