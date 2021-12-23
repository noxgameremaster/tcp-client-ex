
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

bool ServerTask::ForwardPacket(std::unique_ptr<NetPacket> &&forwardPacket)
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    ServerTaskManager *taskman = dynamic_cast<ServerTaskManager *>(parent);

    if (taskman == nullptr)
        return false;

    taskman->Enqueue(std::move(forwardPacket), ServerTaskManager::TaskIOType::OUT);
    return true;
}
