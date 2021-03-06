
#include "taskthread.h"
#include "taskmanager.h"
#include "netpacket.h"
#include "eventThread.h"

TaskThread::TaskThread(NetObject *parent)
    : AbstractTask(parent), m_lock(new std::mutex)
{
    m_taskThread = std::make_unique<EventThread>(this);

    m_taskThread->SetLocker(m_lock);
    m_taskThread->SetCondition([this]() { return this->IsMessageList(); });
    m_taskThread->SetExecution([this]() { this->Dequeue(); return true; });
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

    do
    {
        {
            std::unique_lock<std::mutex> lock(*m_lock);
            if (m_msglist.empty())
                break;

            msg = std::move(m_msglist.front());
            m_msglist.pop_front();
        }

        ExecuteTask(std::move(msg));
    }
    while (true);
}

void TaskThread::DoTask(std::unique_ptr<NetPacket> &&)
{ }

void TaskThread::PushBack(std::unique_ptr<NetPacket> &&msg)
{
    {
        std::lock_guard<std::mutex> lock(*m_lock);

        m_msglist.push_back(std::move(msg));
    }
    m_taskThread->Notify();
}

void TaskThread::StopThread()
{
    m_taskThread->Shutdown();
}

void TaskThread::RunThread()
{
    m_taskThread->Startup();
}


