
#include "serverChatTask.h"
#include "chatPacket.h"
#include "printUtil.h"
#include "stringHelper.h"

using namespace _StringHelper;

ServerChatTask::ServerChatTask(NetObject *parent)
    : ServerTask(parent)
{ }

ServerChatTask::~ServerChatTask()
{ }

bool ServerChatTask::CheckValidColor(uint8_t colrbyte)
{
    return ((colrbyte > static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MIN)) && (colrbyte < static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MAX)));
}

void ServerChatTask::ShowAll(const std::string &message)
{
    std::unique_ptr<ChatPacket> chatmsg(new ChatPacket);

    chatmsg->SetChatMessage(message);
    chatmsg->SetColorId(static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_GREY));

    ForwardPacket(std::move(chatmsg));
}

void ServerChatTask::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    ChatPacket *chat = dynamic_cast<ChatPacket *>(packet.get());

    if (chat == nullptr)
        return;

    std::string msg = stringFormat("message: %s", chat->GetChatMessage());

    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_LIGHTGREEN, msg);

    std::string sendmsg = stringFormat("%d sent a message: %s", static_cast<int>(chat->SenderSocketId()), chat->GetChatMessage());

    ShowAll(sendmsg);
}

std::string ServerChatTask::TaskName() const
{
    return NetPacket::TaskKey<ChatPacket>::Get();
}

