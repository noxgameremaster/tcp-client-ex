
#include "serverTaskThread.h"
#include "serverTaskManager.h"
#include "netpacket.h"
#include "loopThread.h"

ServerTaskThread::ServerTaskThread(NetObject *parent)
    : ServerTask(parent)
{
    m_taskThread = std::make_unique<LoopThread>();
    m_taskThread->SetTaskFunction([this]() { return this->Dequeue(); });
    m_taskThread->SetWaitCondition([this]() { return this->IsMessageList(); });
}

ServerTaskThread::~ServerTaskThread()
{ }

bool ServerTaskThread::IsMessageList() const
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        return m_msglist.size() != 0;
    }
}

void ServerTaskThread::ExecuteTask(std::unique_ptr<NetPacket> &&msg)
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return;

    ServerTaskManager *taskmanager = dynamic_cast<ServerTaskManager *>(parent);

    if (taskmanager == nullptr)
        return;

    ServerTask *task = taskmanager->GetTask(msg->ClassName());

    if (task == nullptr)
        return;

    ExecuteDoTask(task, std::move(msg));
}

bool ServerTaskThread::Dequeue()
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
        ExecuteTask(std::move(msg));
    }
    return true;
}

void ServerTaskThread::DoTask(std::unique_ptr<NetPacket> &&)
{ }

void ServerTaskThread::PushBack(std::unique_ptr<NetPacket> &&msg)
{
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_msglist.push_back(std::move(msg));
    }
    m_taskThread->Notify();
}

void ServerTaskThread::StopThread()
{
    m_taskThread->Shutdown();
}

void ServerTaskThread::RunThread()
{
    m_taskThread->Startup();
}
