
#include "loopThread.h"

LoopThread::LoopThread(NetObject *parent)
    : NetService(parent)
{
    m_terminated = false;
    m_waitCondition = [this]() { return m_terminated; };
}

LoopThread::~LoopThread()
{ }

void LoopThread::WaitCondition()
{
    std::unique_lock<std::mutex> waitLock(m_waitLock);
    m_condvar.wait(waitLock, m_waitCondition);
}

void LoopThread::DoTask(std::function<void()> task)
{
    do
    {
        if (m_waitFunction)
            m_waitFunction();

        if (m_terminated)
            break;

        task();
    }
    while (true);
    //m_OnTerminatedThread.Emit();
}

bool LoopThread::OnInitialize()
{
    if (!m_taskFunction)
        return false;

    return true;
}

void LoopThread::OnDeinitialize()
{ }

bool LoopThread::OnStarted()
{
    m_thread = std::thread([this, task = std::move(m_taskFunction)]() mutable { this->DoTask(std::move(task)); });

    return true;
}

void LoopThread::OnStopped()
{
    if (m_terminated)
        return;

    if (m_thread.joinable())
    {
        m_terminated = true;
        if (m_waitFunction)
            m_condvar.notify_one();
        m_thread.join();
    }
}

void LoopThread::SetWaitCondition(std::function<bool()> &&cond)
{
    m_waitCondition = [this, cond = std::forward<std::remove_reference<decltype(cond)>::type>(cond)]()
    {
        return m_terminated || cond();
    };
    m_waitFunction = [this]()
    {
        this->WaitCondition();
    };
}

void LoopThread::SetTaskFunction(std::function<void()> &&task)
{
    m_taskFunction = std::forward<std::remove_reference<decltype(task)>::type>(task);
}

void LoopThread::Notify()
{
    if (m_waitFunction)
        m_condvar.notify_one();
}