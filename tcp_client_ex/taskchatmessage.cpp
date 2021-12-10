
#include "taskchatmessage.h"
#include "chatpacket.h"
#include "stringhelper.h"
#include "printutil.h"

using namespace _StringHelper;

TaskChatMessage::TaskChatMessage(NetObject *parent)
    : AbstractTask(parent)
{ }

TaskChatMessage::~TaskChatMessage()
{ }

bool TaskChatMessage::CheckValidColor(uint8_t colrbyte)
{
    return ((colrbyte > static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MIN)) && (colrbyte < static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MAX)));
}

void TaskChatMessage::PrintMessage(const std::string &message, uint8_t colr)
{
    std::string form = stringFormat("message: %s", message);

    if (CheckValidColor(colr))
        PrintUtil::PrintMessage(static_cast<PrintUtil::ConsoleColor>(colr), form);
    else
        PrintUtil::PrintMessage(form);
}

void TaskChatMessage::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    ChatPacket *chat = dynamic_cast<ChatPacket *>(packet.get());

    if (chat == nullptr)
    {
        return;
    }

    PrintMessage(chat->GetChatMessage(), chat->GetColorId());
}

std::string TaskChatMessage::TaskName() const
{
    return NetPacket::TaskKey<ChatPacket>::Get();
}

