
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

    m_halted = std::make_shared<std::atomic<bool>>(false);
    m_asyncTask = std::async(std::launch::async, [this]() { return this->Running(this->m_halted); });
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
        *m_halted = true;
        m_halted.reset();

        m_condvar.notify_all();
        m_asyncTask.get();
    }
}

void EventThread::OnStopped()
{ }

bool EventThread::Running(std::shared_ptr<std::atomic<bool>> halted)
{
    do
    {
        {
            auto getlock = m_lock.expired() ? nullptr : m_lock.lock();

            {
                std::unique_lock<std::mutex> lock(*getlock);

                m_condvar.wait(lock, [this, halted]() { return (*halted == true) || m_condition(); });
                if (*halted == true)
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

