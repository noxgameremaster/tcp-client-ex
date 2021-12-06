#include "makepacket.h"
#include <ws2tcpip.h>

MakePacket::MakePacket()
    : BinaryStream(1)
{ }

MakePacket::~MakePacket()
{ }

size_t MakePacket::HeaderLength() const
{
    return sizeof(packet_stx) + sizeof(int) + sizeof(packet_chat_type) + sizeof(packet_etx);
}

bool MakePacket::Make(const std::string &msg)
{
    uint8_t msgLength = static_cast<uint8_t>(msg.length());
    size_t packetLength = msgLength + HeaderLength() + sizeof(msgLength);
    
    SetBufferSize(packetLength);
    try
    {
        WriteCtx(packet_stx);
        WriteCtx(packetLength);
        WriteCtx(packet_chat_type);
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

bool MakePacket::NetSendAll(int clientsocket, const std::string &msg)
{
    if (!Make(msg))
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

