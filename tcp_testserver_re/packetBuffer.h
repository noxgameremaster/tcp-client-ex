
/// THIS IS PACKET BUFFER
/// THE PACKET WILL BE REVEALED BY THIS OBJECT
/// 

#ifndef PACKET_BUFFER_H__
#define PACKET_BUFFER_H__

#include "binarystream.h"
#include <vector>

class WinSocket;
class HeaderData;
class NetPacket;

class PacketBuffer : public BinaryStream
{
    static constexpr size_t max_buffer_length = 65536;
private:
    std::vector<uint8_t> m_buffer;
    size_t m_readSeekpoint;
    size_t m_writeSeekpoint;
    socket_type m_latestSocketId;
    std::unique_ptr<HeaderData> m_headerData;
    std::unique_ptr<NetPacket> m_createdPacket;
    std::list<std::unique_ptr<NetPacket>> m_packetList;

public:
    explicit PacketBuffer();
    ~PacketBuffer() override;

private:
    bool ResetSeekPoint();
    template <class Ty>
    bool ReadChunk(Ty &dest)
    {
        if (!GetStreamChunk(dest, m_readSeekpoint))
            return false;
        m_readSeekpoint += sizeof(dest);
        return true;
    }
    template <class Ty>
    bool WriteChunk(const Ty &src)
    {
        if (!SetStreamChunk(src, m_writeSeekpoint))
            return false;
        m_writeSeekpoint += sizeof(src);
        return true;
    }

    bool CheckCapacity(const size_t &inputSize);
    bool AppendSenderInfo(WinSocket *sock);

public:
    bool PushBack(WinSocket *sock, const std::vector<uint8_t> &stream);

private:
    bool Pulling(const size_t &off);
    std::unique_ptr<NetPacket> PacketInstance();
    bool MakePacketReal(const size_t &off);
    bool MakePacketHeaderData(const size_t &startOff/*, const size_t &length*/);
    bool ReadSenderInfo();
    bool ReadPacketInfo();
    bool PopAsPacket();

public:
    bool IsEmpty() const;
    bool PopPacket(std::unique_ptr<NetPacket> &dest);
};

#endif

