
/// THIS IS PACKET BUFFER
/// THE PACKET WILL BE REVEALED BY THIS OBJECT
/// 

#ifndef PACKET_BUFFER_H__
#define PACKET_BUFFER_H__

#include "binarystream.h"

class WinSocket;
class HeaderData;
class NetPacket;

class PacketBuffer : public BinaryStream
{
    using packet_instance_type = std::unique_ptr<NetPacket>;
    using packet_instance_function = std::function<packet_instance_type(uint8_t)>;

    static constexpr uint32_t sender_field_front = 0x89abcdef;
    static constexpr uint32_t sender_field_back = 0x56781234;
    static constexpr size_t max_buffer_length = 65536;
private:
    std::vector<uint8_t> m_buffer;
    size_t m_readSeekpoint;
    size_t m_writeSeekpoint;
    socket_type m_latestSocketId;
    std::unique_ptr<HeaderData> m_headerData;
    std::unique_ptr<NetPacket> m_createdPacket;
    std::list<std::unique_ptr<NetPacket>> m_packetList;
    packet_instance_function m_instanceFunction;

public:
    explicit PacketBuffer();
    ~PacketBuffer() override;

private:
    bool ResetSeekPoint();
    template <class Ty>
    bool PeekChunk(Ty &dest)
    {
        if (m_readSeekpoint + sizeof(dest) > m_writeSeekpoint)
            return false;

        return GetStreamChunk(dest, m_readSeekpoint);
    }

    template <class Ty>
    bool ReadChunk(Ty &dest)
    {
        if (m_readSeekpoint + sizeof(dest) > m_writeSeekpoint)
            return false;

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
    void SetInstanceFunction(packet_instance_function &&f)
    {
        m_instanceFunction = std::forward<std::remove_reference<decltype(f)>::type>(f);
    }

private:
    bool Pulling(size_t off);
    std::unique_ptr<NetPacket> PacketInstance();
    bool MakePacketReal(const size_t &off);
    bool MakePacketHeaderData(const size_t &startOff/*, const size_t &length*/);
    bool ReadSenderInfo();
    bool ReadPacketInfo();
    bool PopAsPacket();

public:
    bool IsEmpty() const;
    bool PopPacket(std::unique_ptr<NetPacket> &dest);

private:
    mutable std::mutex m_lock;
};

#endif

