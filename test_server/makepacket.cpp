#include "makepacket.h"
#include "eventworker.h"
#include <list>
#include <ws2tcpip.h>

MakePacket::MakePacket()
    : BinaryStream(16384)
{ }

MakePacket::~MakePacket()
{ }

size_t MakePacket::HeaderLength() const
{
    return sizeof(packet_stx) + sizeof(int) + sizeof(packet_chat_type) + sizeof(packet_etx);
}

bool MakePacket::MakeChat(const std::string &msg, const uint8_t &messageColor)
{
    uint8_t msgLength = static_cast<uint8_t>(msg.length());
    size_t packetLength = msgLength + HeaderLength() + sizeof(msgLength) + sizeof(messageColor);
    
    SetBufferSize(packetLength);
    try
    {
        WriteCtx(packet_stx);
        WriteCtx(packetLength);
        WriteCtx(packet_chat_type);
        WriteCtx(messageColor);
        WriteCtx(msgLength);
        
        for (const char c : msg)
            WriteCtx(c);
        WriteCtx(packet_etx);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool MakePacket::MakeEcho(const std::string &echoMessage)
{
    size_t packetLength = HeaderLength() + echoMessage.length() + sizeof(char);

    SetBufferSize(packetLength);
    try
    {
        WriteCtx(packet_stx);
        WriteCtx(packetLength);
        WriteCtx(static_cast<uint8_t>(2));
        WriteCtx(static_cast<uint8_t>(echoMessage.length()));
        for (const char c : echoMessage)
            WriteCtx(c);
        WriteCtx(packet_etx);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool MakePacket::MakeFileMeta(const std::string &filename, const std::string &path)
{
    uint8_t filenameLength = static_cast<uint8_t>(filename.length());
    uint8_t pathLength = static_cast<uint8_t>(path.length());
    size_t filesize = 50;
    size_t packetLength = HeaderLength() + sizeof(filenameLength) + filenameLength + sizeof(pathLength) + pathLength + sizeof(filesize);

    SetBufferSize(packetLength);
    try
    {
        WriteCtx(packet_stx);
        WriteCtx(packetLength);
        WriteCtx(static_cast<uint8_t>(4));
        WriteCtx(filenameLength);
        for (const auto &c : filename)
            WriteCtx(c);
        WriteCtx(pathLength);
        for (const auto &c : path)
            WriteCtx(c);
        WriteCtx(filesize);
        WriteCtx(packet_etx);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool MakePacket::SendImpl(int sendsock, const uint8_t *stream, const size_t &length)
{
    int cur = 0, maxbyte = static_cast<int>(length);

    while (cur < maxbyte)
    {
        int sendbyte = send(sendsock, reinterpret_cast<const char *>(stream), length, 0);

        if (sendbyte < 0)
            return false;

        cur += sendbyte;
    }
    m_OnSended.Emit(stream, length);
    return true;
}

bool MakePacket::NetSendAll(int clientsocket, const std::string &msg, const uint8_t colr)
{
    if (!MakeChat(msg, colr))
        return false;

    int sendbyte = 0;
    int packetLength = static_cast<int>(BufferSize());

    while (sendbyte < packetLength)
    {
        int sendCur = send(clientsocket, reinterpret_cast<const char *>(GetRaw()), packetLength, 0);

        if (sendCur <= 0)
            return false;

        sendbyte += sendCur;
    }
    return true;
}

bool MakePacket::NetEchoSend(int clientsocket)
{
    if (!MakeEcho("push echo message"))
        return false;

    int sendbyte = 0;
    int packetLength = static_cast<int>(BufferSize());

    while (sendbyte < packetLength)
    {
        int sendCur = send(clientsocket, reinterpret_cast<const char *>(GetRaw()), packetLength, 0);

        if (sendCur <= 0)
            return false;

        sendbyte += sendCur;
    }
    return true;
}

bool MakePacket::ReadChatPacket(int senderSocket)
{
    uint8_t colr = 0;
    uint8_t length = 0;
    std::string msg(256, 0);

    try
    {
        ReadCtx(colr);
        ReadCtx(length);
        msg.resize(length);
        for (auto &c : msg)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    EventWorker::Instance().AppendTask(&m_OnReceiveChat, senderSocket, msg);
    return true;
}

bool MakePacket::ReadEchoPacket(int senderSocket)
{
    uint8_t length = 0;
    std::string msg(256, 0);

    try
    {
        ReadCtx(length);
        msg.resize(length);
        for (auto &c : msg)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    EventWorker::Instance().AppendTask(&m_OnReceiveEcho, senderSocket, msg);
    return true;
}

bool MakePacket::PacketTypeCase(int senderSocket, const uint8_t type)
{
    bool ret = false;
    auto checker = [&ret](bool bCond) { ret = bCond; };

    switch (type)
    {
    case 1: //Chat
        checker(ReadChatPacket(senderSocket));
        break;
    case 2: //Echo
        checker(ReadEchoPacket(senderSocket));
        break;
    default:
        EventWorker::Instance().AppendTask(&m_OnUnknownPacketType, senderSocket, type);
        return false;
    }
    return ret;
}

bool MakePacket::ReadPacket(int senderSocket, const char *buffer, const size_t &length)
{
    //std::lock_guard<std::mutex> guard(m_lock);
    PutStreamRaw(reinterpret_cast<const uint8_t *>(buffer), length);

    int stx = 0;
    size_t packetLength = 0;
    uint8_t packetId = 0;

    try
    {
        ReadCtx(stx);
        if (stx != packet_stx)
        {
            std::unique_ptr<char[]> copy(new char[length]);

            memcpy_s(copy.get(), length, buffer, length);
            //OnReceiveUnknownPacket(buffer, length);
            //EventWorker::Instance().AppendTask(m_OnReceiveUnknown, std::move(copy), length);
            m_OnReceiveUnknown.Emit(senderSocket, std::move(copy), length);
            return false;
        }
        ReadCtx(packetLength);
        ReadCtx(packetId);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return PacketTypeCase(senderSocket, packetId);
}

std::string MakePacket::filterPrint(const char *str, const size_t &length)
{
    std::list<char> buffer;

    size_t pos = 0;
    for (; pos < length; ++pos)
    {
        if (!(str[pos] & 0x80) && str[pos])
            buffer.push_back(str[pos]);
    }

    return std::string(buffer.cbegin(), buffer.cend());
}
