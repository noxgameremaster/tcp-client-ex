
#ifndef PACKET_BUFFER_FIX_H__
#define PACKET_BUFFER_FIX_H__

#include "binarystream.h"
#include <map>

class WinSocket;
class NetPacket;
class HeaderData;
class BinaryBuffer;

class PacketBufferFix : public BinaryStream
{
    using packet_instance_type = std::unique_ptr<NetPacket>;
    using packet_instance_function = std::function<packet_instance_type(uint8_t)>;
    using parse_action = std::function<bool()>;

    struct SenderInfo;

private:
    std::vector<uint8_t> m_buffer;
    std::unique_ptr<SenderInfo> m_senderInfo;
    std::unique_ptr<HeaderData> m_headerInfo;

    uint32_t m_readSeekpoint;
    uint32_t m_writeSeekpoint;

    parse_action m_parseAction;
    std::unique_ptr<BinaryBuffer> m_tempBuffer;

    packet_instance_function m_instancePacketFunction;
    packet_instance_type m_createdPacket;
    std::list<packet_instance_type> m_packetList;

public:
    explicit PacketBufferFix();
    ~PacketBufferFix();

private:
    bool CheckCapacity(const size_t &inputSize);
    bool AppendSenderInfo(WinSocket *sock);

    template <class Ty>
    bool PeekChunk(Ty &dest)
    {
        dest ^= dest;
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

    void RewindSeek(const size_t rewindCount);
    std::unique_ptr<NetPacket> InstancePacket();
    bool MakePacketImpl();
    bool MakePacketHeaderData();

    bool EvacuateChunk(const size_t count);

    bool ReadSendInfoDetail();

    bool ReadSendInfo();
    bool ReadPacketDetail();
    bool ReadPacketEtc();
    bool ReadPacketLength();
    bool ReadStartpoint();
    void ReadStream();

public:
    void SetInstanceFunction(packet_instance_function &&f)
    {
        m_instancePacketFunction = std::forward<std::remove_reference<decltype(f)>::type>(f);
    }
    bool PushBack(WinSocket *sock, const std::vector<uint8_t> &stream);
    bool IsEmpty() const;
    bool PopPacket(std::unique_ptr<NetPacket> &dest);

private:
    mutable std::mutex m_lock;
};

#endif

