
#include "abstracttask.h"

AbstractTask::AbstractTask(NetObject *parent)
    : NetObject(parent)
{ }

AbstractTask::~AbstractTask()
{ }

void AbstractTask::ExcuteDoTask(AbstractTask *task, std::unique_ptr<NetPacket> &&packet)
{
    task->DoTask(std::move(packet));
}

