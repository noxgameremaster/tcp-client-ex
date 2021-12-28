
#include "serverTask.h"
#include "serverTaskManager.h"

ServerTask::ServerTask(NetObject *parent)
    : NetObject(parent)
{ }

ServerTask::~ServerTask()
{ }

void ServerTask::ExecuteDoTask(ServerTask *task, std::unique_ptr<NetPacket> &&packet)
{
    task->DoTask(std::move(packet));
}

bool ServerTask::ForwardPacketToManager(std::unique_ptr<NetPacket> &&packet, bool toSelf)
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    ServerTaskManager *taskman = dynamic_cast<ServerTaskManager *>(parent);

    if (taskman == nullptr)
        return false;

    taskman->Enqueue(std::move(packet), toSelf ? ServerTaskManager::TaskIOType::IN : ServerTaskManager::TaskIOType::OUT);
    return true;
}

bool ServerTask::ForwardPacket(std::unique_ptr<NetPacket> &&forwardPacket)
{
    return ForwardPacketToManager(std::move(forwardPacket));
}

bool ServerTask::SendBackPacket(std::unique_ptr<NetPacket> &&comebackPacket)
{
    return ForwardPacketToManager(std::move(comebackPacket), true);
}
