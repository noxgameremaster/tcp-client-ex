
#include "taskecho.h"
#include "taskmanager.h"
#include "echopacket.h"
#include "stringhelper.h"
#include "printutil.h"

using namespace _StringHelper;

TaskEcho::TaskEcho(NetObject *parent)
    : AbstractTask(parent)
{ }

TaskEcho::~TaskEcho()
{ }

void TaskEcho::ReversalEchoPacket()
{
    std::unique_ptr<EchoPacket> echoPacket(new EchoPacket);

    echoPacket->SetEchoMessage("echo response...");

    TaskManager *taskman = dynamic_cast<TaskManager *>(GetParent());

    if (taskman == nullptr)
        return;

    taskman->ForwardPacket(std::move(echoPacket));
}

void TaskEcho::PrintEchoMessage(const std::string &echoMessage) const
{
    PrintUtil::PrintMessage(stringFormat("echo message: %s", echoMessage));
}

void TaskEcho::DoTask(std::unique_ptr<NetPacket>&& packet)
{
    EchoPacket *echo = dynamic_cast<EchoPacket *>(packet.get());

    if (echo == nullptr)
        return;

    PrintEchoMessage(echo->GetEchoMessage());
    ReversalEchoPacket();
}

std::string TaskEcho::TaskName() const
{
    return NetPacket::TaskKey<EchoPacket>::Get();
}
