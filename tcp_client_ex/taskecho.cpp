
#include "taskecho.h"
#include "echopacket.h"
#include "stringhelper.h"
#include "netLogObject.h"

using namespace _StringHelper;

TaskEcho::TaskEcho(NetObject *parent)
    : AbstractTask(parent)
{ }

TaskEcho::~TaskEcho()
{ }

void TaskEcho::ReversalEchoPacket()
{
    std::unique_ptr<EchoPacket> echoPacket(new EchoPacket);

    echoPacket->SetEchoMessage(stringFormat("echo response...msg: %s", m_echoMessage));
    ForwardPacketToManager(std::move(echoPacket));
}

void TaskEcho::PrintEchoMessage(const std::string &echoMessage) const
{
    //PrintUtil::PrintMessage(stringFormat("echo message: %s", echoMessage));
    NetLogObject::LogObject().AppendLogMessage(stringFormat("echo message: %s", echoMessage));
}

void TaskEcho::DoTask(std::unique_ptr<NetPacket>&& packet)
{
    EchoPacket *echo = dynamic_cast<EchoPacket *>(packet.get());

    if (echo == nullptr)
        return;

    m_echoMessage = echo->GetEchoMessage();
    PrintEchoMessage(m_echoMessage);
    ReversalEchoPacket();
}

std::string TaskEcho::TaskName() const
{
    return NetPacket::TaskKey<EchoPacket>::Get();
}

