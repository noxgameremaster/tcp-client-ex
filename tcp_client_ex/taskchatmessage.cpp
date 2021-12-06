
#include "taskchatmessage.h"
#include "chatpacket.h"
#include "stringhelper.h"
#include <iostream>
#include <windows.h>

using namespace _StringHelper;

TaskChatMessage::TaskChatMessage(NetObject *parent)
    : AbstractTask(parent)
{
    m_consoleHandle = nullptr;
    m_oldColor = ConsoleColor::COLOR_DARKWHITE;
}

TaskChatMessage::~TaskChatMessage()
{ }

void TaskChatMessage::ChangeTextColor(ConsoleColor colr)
{
    SetConsoleTextAttribute(m_consoleHandle, static_cast<WORD>(colr));
}

void TaskChatMessage::GetConsoleHandlePointer()
{
    m_consoleHandle = ::GetStdHandle(STD_INPUT_HANDLE);
}

bool TaskChatMessage::CheckValidColor(uint8_t colrbyte)
{
    return ((colrbyte > static_cast<uint8_t>(ConsoleColor::COLOR_MIN)) && (colrbyte < static_cast<uint8_t>(ConsoleColor::COLOR_MAX)));
}

void TaskChatMessage::PrintMessage(const std::string &message, ConsoleColor colr)
{
    ChangeTextColor(colr);
    std::cout << stringFormat("message: %s\n", message);
    ChangeTextColor(m_oldColor);
}

void TaskChatMessage::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    ChatPacket *chat = dynamic_cast<ChatPacket *>(packet.get());

    if (chat == nullptr)
        return;

    ConsoleColor textcolr = m_oldColor;

    if (CheckValidColor(chat->GetColorId()))
        textcolr = static_cast<ConsoleColor>(chat->GetColorId());

    PrintMessage(chat->GetChatMessage(), textcolr);
}

std::string TaskChatMessage::TaskName()
{
    return ChatPacket::TaskName();
}

