
#include "chatpacket.h"
#include "packetordertable.h"
#include <iostream>

ChatPacket::ChatPacket()
    : NetPacket()
{
    m_messageLength = 0;
    m_messageColor = 0;
}

ChatPacket::~ChatPacket()
{
}

size_t ChatPacket::PacketSize()
{
    return sizeof(m_messageColor) + sizeof(m_messageLength) + m_messageLength;
}

bool ChatPacket::ReadMessage()
{
    static constexpr size_t buffer_size = sizeof(m_messageLength) << 8;
    char buffer[buffer_size] = { };
    int length = static_cast<int>(m_messageLength);
    int pos = 0;

    try
    {
        while ((--length) >= 0)
            ReadCtx(buffer[pos++]);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    m_message = buffer;
    return true;
}

bool ChatPacket::OnReadPacket()
{
    try
    {
        ReadCtx(m_messageColor);
        ReadCtx(m_messageLength);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    if (m_messageLength == 0 || (m_messageLength & 0x80))
        return false;

    return ReadMessage();
}

void ChatPacket::DoAction()
{
    std::cout << "ChatPacket::DoAction:: " << m_message << std::endl;
}

uint8_t ChatPacket::GetPacketId()
{
    return static_cast<uint8_t>(PacketOrderTable<ChatPacket>::GetId());
}

