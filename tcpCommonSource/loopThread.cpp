
#include "loopThread.h"
#include "netLogObject.h"
#include "stringHelper.h"

using namespace _StringHelper;

LoopThread::LoopThread(NetObject *owner)
    : NetService(owner)
{ }

LoopThread::~LoopThread()
{ }

void LoopThread::ThreadHalted()
{
    //Debug only...
    NetObject *owner = GetParent();

    NET_PUSH_LOGMSG(stringFormat("the class %s was halted.", owner == nullptr ? "null" : owner->ObjectName()));
}

void LoopThread::DoTask(task_function_type task, std::shared_ptr<std::atomic<bool>> halted)
{
    do
    {
        if (*halted)
            break;

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
    m_halted = std::make_shared<std::atomic<bool>>(false);
    m_thread = std::thread([this, task = std::move(m_taskFunction)]() mutable { this->DoTask(std::move(task), this->m_halted); });

    return true;
}

void LoopThread::OnStopped()
{
    if (m_thread.joinable())
    {
        *m_halted = true;
        m_halted.reset();
        m_thread.join();

        auto owner = GetParent();

        NET_PUSH_LOGMSG(stringFormat("the class %s stop request...", owner == nullptr ? "null" : owner->ObjectName()));
    }
}

void LoopThread::SetTaskFunction(task_function_type &&task)
{
    m_taskFunction = std::forward<std::remove_reference<decltype(task)>::type>(task);
}

