
#include "abstracttask.h"
#include "taskmanager.h"

AbstractTask::AbstractTask(NetObject *parent)
    : NetObject(parent)
{ }

AbstractTask::~AbstractTask()
{ }

void AbstractTask::ExecuteDoTask(AbstractTask *task, std::unique_ptr<NetPacket> &&packet)
{
    task->DoTask(std::move(packet));
}

bool AbstractTask::ForwardPacketToManager(std::unique_ptr<NetPacket> &&forwardPacket, bool toInner)
{
    NetObject *parent = GetParent();

    if (nullptr == parent)
        return false;

    TaskManager *taskman = dynamic_cast<TaskManager *>(parent);

    if (nullptr == taskman)
        return false;

    taskman->ForwardPacket(std::forward<std::remove_reference<decltype(forwardPacket)>::type>(forwardPacket), toInner);
    return true;
}



