
#include "loopThread.h"
#include "netLogObject.h"
#include "stringHelper.h"

using namespace _StringHelper;

LoopThread::LoopThread(NetObject *owner)
    : NetService(owner)
{
    m_terminated = false;
    m_waitCondition = [this]() { return this->m_terminated; };
    m_useCondition = false;
}

LoopThread::~LoopThread()
{ }

void LoopThread::WaitCondition()
{
    std::unique_lock<std::mutex> waitLock(m_waitLock);
    m_condvar.wait(waitLock, m_waitCondition);
}

void LoopThread::ThreadHalted()
{
    //Debug only...
    NetObject *owner = GetParent();

    NET_PUSH_LOGMSG(stringFormat("the class %s was halted.", owner == nullptr ? "null" : owner->ObjectName()));
}

void LoopThread::DoTask(task_function_type task)
{
    do
    {
        if (m_terminated)
            break;

        if (!task())
            break;
    }
    while (true);
    //m_OnTerminatedThread.Emit();
    ThreadHalted();
}

void LoopThread::DoTaskCondition(task_function_type task)
{
    do
    {
        {
            std::unique_lock<std::mutex> waitLock(m_waitLock);
            m_condvar.wait(waitLock, m_waitCondition);

            if (m_terminated)
                break;
        }
        if (!task())
            break;
    }
    while (true);
    ThreadHalted();
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
    m_thread = m_useCondition ?
        std::thread([this, task = std::move(m_taskFunction)]() mutable { this->DoTaskCondition(std::move(task)); }) :
        std::thread([this, task = std::move(m_taskFunction)]() mutable { this->DoTask(std::move(task)); });

    return true;
}

void LoopThread::OnStopped()
{
    if (m_terminated)
        return;

    if (m_thread.joinable())
    {
        m_terminated = true;
        if (m_useCondition)
            m_condvar.notify_one();
        m_thread.join();

        auto owner = GetParent();

        NET_PUSH_LOGMSG(stringFormat("the class %s stop request...", owner == nullptr ? "null" : owner->ObjectName()));
    }
}

void LoopThread::SetWaitCondition(std::function<bool()> &&cond)
{
    m_useCondition = true;
    m_waitCondition = [this, cond = std::forward<std::remove_reference<decltype(cond)>::type>(cond)]()
    {
        return m_terminated || cond();
    };
    m_waitFunction = [this]()
    {
        this->WaitCondition();
    };
}

void LoopThread::SetTaskFunction(task_function_type &&task)
{
    m_taskFunction = std::forward<std::remove_reference<decltype(task)>::type>(task);
}

void LoopThread::Notify()
{
    if (m_useCondition)
        m_condvar.notify_one();
}