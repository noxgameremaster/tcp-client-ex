
#include "taskmanager.h"
#include "taskthread.h"
#include "taskchatmessage.h"
#include "taskecho.h"
#include "iobuffer.h"

TaskManager::TaskManager(NetObject *parent)
    : NetService(parent)
{
}

TaskManager::~TaskManager()
{
}

bool TaskManager::OnInitialize()
{   //테스크 등록
    m_taskmap.emplace(TaskChatMessage::TaskName(), std::shared_ptr<TaskChatMessage>());
    m_taskmap.emplace(TaskEcho::TaskName(), std::shared_ptr<TaskEcho>());
    
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

