
#include "taskmanager.h"
#include "netflowcontrol.h"
#include "taskthread.h"
#include "taskchatmessage.h"
#include "taskecho.h"
#include "iobuffer.h"
#include "echopacket.h"

TaskManager::TaskManager(NetObject *parent)
    : NetService(parent)
{
    m_netFlow = nullptr;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::SetNetFlowIO()
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    m_netFlow = dynamic_cast<NetFlowControl *>(parent);
    return (m_netFlow != nullptr);
}

bool TaskManager::OnInitialize()
{   //테스크 등록
    if (!SetNetFlowIO())
        return false;

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
    SendOnInitial();
    m_taskthread->RunThread();
    return true;
}

void TaskManager::OnStopped()
{
    m_taskthread->StopThread();
}

void TaskManager::SendOnInitial()
{
    std::unique_ptr<EchoPacket> packet(new EchoPacket);

    packet->SetEchoMessage("connect completed");
    m_taskthread->PushBack(std::move(packet));
}

void TaskManager::InputTask(std::unique_ptr<NetPacket> &&packet)
{
    if (m_taskthread)
        m_taskthread->PushBack(std::move(packet));
}

AbstractTask *TaskManager::GetTask(const std::string &taskName)
{
    auto taskIterator = m_taskmap.find(taskName);

    if (taskIterator == m_taskmap.cend())
        return nullptr;

    return taskIterator->second.get();
}

void TaskManager::ForwardPacket(std::unique_ptr<NetPacket>&& packet)
{
    if (GetParent()!=nullptr)
        m_netFlow->Enqueue(std::move(packet), NetFlowControl::IOType::OUT);
}


