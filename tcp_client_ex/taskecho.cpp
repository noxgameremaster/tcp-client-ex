
#include "taskecho.h"
#include "echopacket.h"
#include "stringhelper.h"
#include <iostream>

using namespace _StringHelper;

TaskEcho::TaskEcho()
    : AbstractTask()
{ }

TaskEcho::~TaskEcho()
{ }

void TaskEcho::PrintEchoMessage(const std::string &echoMessage) const
{
    std::cout << stringFormat("echo message: %s\n", echoMessage);
}

void TaskEcho::DoTask(std::unique_ptr<NetPacket>&& packet)
{
    EchoPacket *echo = dynamic_cast<EchoPacket *>(packet.get());

    if (echo == nullptr)
        return;

    PrintEchoMessage(echo->GetEchoMessage());

    //Todo. send here
}

std::string TaskEcho::TaskName()
{
    return EchoPacket::TaskName();
}

