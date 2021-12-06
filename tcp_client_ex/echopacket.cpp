
#include "echopacket.h"
#include "packetordertable.h"

EchoPacket::EchoPacket()
    : NetPacket()
{ }

EchoPacket::~EchoPacket()
{ }

bool EchoPacket::ReadEchoMessage()
{
    char buffer[80] = { 0, };
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

bool EchoPacket::OnReadPacket()
{
    try
    {
        ReadCtx(m_messageLength);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    if ((m_messageLength == 0) || (m_messageLength & 0x80))
        return false;

    if (!ReadEchoMessage())
        return false;

    return true;
}

bool EchoPacket::OnWritePacket()
{
    m_messageLength = static_cast<char>(m_message.length());

    try
    {
        WriteCtx(m_messageLength);
        for (const char &c : m_message)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    return true;
}

uint8_t EchoPacket::GetPacketId()
{
    return static_cast<uint8_t>(PacketOrderTable<EchoPacket>::GetId());
}

