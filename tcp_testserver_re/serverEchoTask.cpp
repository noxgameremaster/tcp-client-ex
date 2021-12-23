
#include "serverEchoTask.h"
#include "echoPacket.h"
#include "chatPacket.h"
#include "printUtil.h"

#include "stringHelper.h"

using namespace _StringHelper;

ServerEchoTask::ServerEchoTask(NetObject *parent)
    : ServerTask(parent)
{ }

ServerEchoTask::~ServerEchoTask()
{ }

void ServerEchoTask::SendEchoResponse(const socket_type &senderId, const std::string &echoMsg)
{
    std::unique_ptr<ChatPacket> msg(new ChatPacket);

    msg->SetChatMessage(stringFormat("server response: %s", echoMsg));
    msg->SetColorId(static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_DARKBLUE));
    msg->SetSenderSocketId(senderId);
}

void ServerEchoTask::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    EchoPacket *echo = dynamic_cast<EchoPacket *>(packet.get());

    if (echo == nullptr)
        return;

    std::string echoMessage = echo->GetEchoMessage();

    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_BLUE, echoMessage);
    SendEchoResponse(packet->SenderSocketId(), echoMessage);
}

std::string ServerEchoTask::TaskName() const
{
    return NetPacket::TaskKey<EchoPacket>::Get();
}

