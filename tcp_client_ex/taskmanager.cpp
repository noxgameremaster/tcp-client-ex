
#include "taskmanager.h"
#include "taskthread.h"
#include "taskchatmessage.h"
#include "taskecho.h"
#include "iobuffer.h"
#include "netpacket.h"

TaskManager::TaskManager(NetObject *parent)
    : NetService(parent)
{
}

TaskManager::~TaskManager()
{
}

bool TaskManager::OnInitialize()
{   //테스크 등록
    m_taskmap.emplace(TaskChatMessage::TaskName(), std::make_shared<TaskChatMessage>(this));
    m_taskmap.emplace(TaskEcho::TaskName(), std::make_shared<TaskEcho>(this));
    
    m_taskthread = std::make_unique<TaskThread>(this);
    return true;
}

void TaskManager::OnDeinitialize()
{
    //
}

bool TaskManager::OnStarted()
{
    return true;
}

void TaskManager::OnStopped()
{
}

void TaskManager::SetSendBuffer(std::shared_ptr<IOBuffer> sendbuffer)
{
    m_sendbuffer = sendbuffer;
}

void TaskManager::InputTask(std::unique_ptr<NetPacket> &&packet)
{
    if (m_taskthread)
    {
        m_taskthread->PushBack(std::move(packet));
    }
}

AbstractTask *TaskManager::GetTask(const std::string &taskName)
{
    auto taskIterator = m_taskmap.find(taskName);

    if (taskIterator == m_taskmap.cend())
        return nullptr;

    return taskIterator->second.get();
}

void TaskManager::SendPacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_sendbuffer.expired())
        return;

    auto sendbuffer = m_sendbuffer.lock();

    std::unique_ptr<NetPacket> willsend = std::forward<std::remove_reference<decltype(packet)>::type>(packet);

    if (!willsend->Write())
        return;

    sendbuffer->PushBuffer(willsend...);
}

