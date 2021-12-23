
#include "chatpacket.h"
#include "packetordertable.h"
#include "stringhelper.h"
#include "printutil.h"

using namespace _StringHelper;

ChatPacket::ChatPacket()
    : NetPacket()
{
    m_messageLength = 0;
    m_messageColor = 0;
}

ChatPacket::~ChatPacket()
{
}

size_t ChatPacket::PacketSize(Mode)
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

bool ChatPacket::OnWritePacket()
{
    if (m_message.length() > 128)
        return false;

    m_messageLength = static_cast<uint8_t>(m_message.length());

    try
    {
        WriteCtx(m_messageColor);
        WriteCtx(m_messageLength);
        for (const auto &c : m_message)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;    
    }
    return true;
}

void ChatPacket::DoAction()
{
    PrintUtil::PrintMessage(stringFormat("ChatPacket::DoAction:: %s", m_message));
}

uint8_t ChatPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<ChatPacket>::GetId());
}

void ChatPacket::SetChatMessage(const std::string &msg)
{
    std::string sendData = msg;

    if (msg.length() > 128)
        sendData.resize(128);

    m_messageLength = static_cast<uint8_t>(sendData.length());
    m_message = std::move(sendData);
}