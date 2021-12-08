
#include "echopacket.h"
#include "packetordertable.h"

EchoPacket::EchoPacket()
    : NetPacket()
{
    m_messageLength = 0;
}

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
    if (m_message.length() > 128)
        return false;

    m_messageLength = static_cast<uint8_t>(m_message.length());

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

void EchoPacket::SetEchoMessage(const std::string &echoMessage)
{
    if (echoMessage.empty() || (echoMessage.length() >= 0x80))
        m_message = "echo completed";
    else
        m_message = echoMessage;

    m_messageLength = static_cast<decltype(m_messageLength)>(m_message.length());
}

size_t EchoPacket::PacketSize(Mode)
{
    return sizeof(m_messageLength) + m_messageLength;
}

