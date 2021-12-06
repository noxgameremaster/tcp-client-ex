
#include "taskthread.h"
#include "taskmanager.h"
#include "netpacket.h"

TaskThread::TaskThread(NetObject *parent)
    : AbstractTask(parent)
{
    m_terminated = false;
}

TaskThread::~TaskThread()
{
}

void TaskThread::ExcuteTask(std::unique_ptr<NetPacket> &&msg)
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

    ExcuteDoTask(task, std::move(msg));
}

void TaskThread::Dequeue()
{
    std::unique_ptr<NetPacket> msg;

    for (;;)
    {
        {
            std::lock_guard<std::mutex> lock(m_lock);

            if (m_msglist.empty())
                break;

            msg = std::move(m_msglist.front());
            m_msglist.pop_front();
        }
        if (!msg)
            continue;
        ExcuteTask(std::move(msg));
    }

}

void TaskThread::DoTask(std::unique_ptr<NetPacket> &&)
{ }

void TaskThread::DoThreadTask()
{
    do
    {
        {
            std::unique_lock<std::mutex> waitLock(m_waitLock);
            m_condvar.wait(waitLock);
        }
        if (m_terminated)
            break;

        Dequeue();
    }
    while (true);
}

void TaskThread::PushBack(std::unique_ptr<NetPacket> &&msg)
{
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_msglist.push_back(std::move(msg));
        m_condvar.notify_one();
    }
}

void TaskThread::StopThread()
{
    m_terminated = true;
    if (m_taskThread.joinable())
    {
        m_condvar.notify_one();
        m_taskThread.join();
    }
}

void TaskThread::RunThread()
{
    m_taskThread = std::thread([this]() { this->DoThreadTask(); });
}


