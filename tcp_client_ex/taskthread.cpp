
#include "taskthread.h"
#include "taskmanager.h"
#include "netpacket.h"
#include "loopThread.h"

TaskThread::TaskThread(NetObject *parent)
    : AbstractTask(parent)
{
    m_terminated = false;
}

TaskThread::~TaskThread()
{ }

bool TaskThread::IsMessageList() const
{
    return m_msglist.size() != 0;
}

void TaskThread::ExecuteTask(std::unique_ptr<NetPacket> &&msg)
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return;

    TaskManager *taskmanager = dynamic_cast<TaskManager *>(parent);

    if (taskmanager == nullptr)
        return;

    AbstractTask *task = taskmanager->GetTask(msg->ClassName());

    if (task == nullptr)
        return;

    ExecuteDoTask(task, std::move(msg));
}

void TaskThread::Dequeue()
{
    std::unique_ptr<NetPacket> msg;

    for (;;)
    {
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_condvar.wait(ulock, [this]() { return this->m_terminated || this->IsMessageList(); });

            if (m_terminated)
                break;

            msg = std::move(m_msglist.front());
            m_msglist.pop_front();
        }
        ExecuteTask(std::move(msg));
    }
}

void TaskThread::DoTask(std::unique_ptr<NetPacket> &&)
{ }

void TaskThread::PushBack(std::unique_ptr<NetPacket> &&msg)
{
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_msglist.push_back(std::move(msg));
    }
    //m_taskThread->Notify();
    m_condvar.notify_one();
}

void TaskThread::StopThread()
{
    m_terminated = true;
    if (m_taskThread.joinable())
    {
        m_condvar.notify_one();
        m_taskThread.join();
    }
    //m_taskThread->Shutdown();
}

void TaskThread::RunThread()
{
    //m_taskThread->Startup();
    m_taskThread = std::thread([this]() { this->Dequeue(); });
}


