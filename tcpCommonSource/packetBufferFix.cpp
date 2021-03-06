
#include "packetBufferFix.h"
#include "binaryBuffer.h"
#include "winsocket.h"
#include "netpacket.h"
#include "headerdata.h"
#include "netLogObject.h"

static socket_type s_latestReceiveSocket = WinSocket::invalid_socket;
static constexpr size_t max_buffer_packet_count = 12;
static constexpr size_t max_buffer_length = NetPacket::packet_unit_max_size * max_buffer_packet_count;

struct PacketBufferFix::SenderInfo
{
    static constexpr uint32_t sender_field_first = 0x89abcdef;
    static constexpr uint32_t sender_field_second = 0x56781234;

    const uint32_t m_sender_field_front = sender_field_first;
    const uint32_t m_sender_field_back = sender_field_second;
    socket_type m_latestSocketId;
};

PacketBufferFix::PacketBufferFix()
    : BinaryStream(1)
{
    m_senderInfo = std::make_unique<SenderInfo>();
    m_headerInfo = std::make_unique<HeaderData>();

    m_senderInfo->m_latestSocketId = WinSocket::invalid_socket;
    m_buffer.resize(max_buffer_length);
    SetContext(&m_buffer);
    
    m_writeSeekpoint = 0;
    m_readSeekpoint = 0;

    m_tempBuffer = std::make_unique<BinaryBuffer>(NetPacket::packet_unit_max_size);   //!FIXME!!//

    m_parseAction = [this]() { return this->ReadStartpoint(); };
}

PacketBufferFix::~PacketBufferFix()
{ }

bool PacketBufferFix::CheckCapacity(const size_t &inputSize)
{
    return (m_writeSeekpoint + inputSize) <= max_buffer_length;
}

bool PacketBufferFix::AppendSenderInfo(WinSocket *sock)
{
    socket_type sockId = sock->GetFd();

    if (s_latestReceiveSocket == sockId)
        return true;
    s_latestReceiveSocket = sockId;

    if (!CheckCapacity(sizeof(SenderInfo)))
        return false;

    WriteChunk(m_senderInfo->m_sender_field_front);
    WriteChunk(m_senderInfo->m_sender_field_back);
    WriteChunk(sockId);
    return true;
}

void PacketBufferFix::RewindSeek(const size_t rewindCount)
{
    m_readSeekpoint -= ((rewindCount > m_readSeekpoint) ? m_readSeekpoint : rewindCount);
}

std::unique_ptr<NetPacket> PacketBufferFix::InstancePacket()
{
    uint8_t packetId = 0;

    if (!m_headerInfo->GetProperty<HeaderData::FieldInfo::MAIN_CMD_TYPE>(packetId))
        return nullptr;

    return m_instancePacketFunction ? m_instancePacketFunction(packetId) : nullptr;
}

bool PacketBufferFix::MakePacketImpl()
{
    auto pack = InstancePacket();

    if (!pack)
        return false;

    m_createdPacket = std::move(pack);

    size_t length = 0;

    m_headerInfo->GetProperty<HeaderData::FieldInfo::LENGTH>(length);

    auto stream = m_tempBuffer->GetPartRe(length/*, m_headerInfo->FieldLength()*/);

    m_createdPacket->SetSenderSocketId(m_senderInfo->m_latestSocketId);
    m_createdPacket->SetHeaderData(std::move(m_headerInfo));
    m_headerInfo = std::make_unique<HeaderData>();
    m_createdPacket->PutStream(stream);

    return m_createdPacket->Read();
}

bool PacketBufferFix::MakePacketHeaderData()
{
    size_t length = m_headerInfo->FieldLength();

    if (!m_headerInfo->PutStream(m_tempBuffer->GetPartRe(length)))
        return false;

    if (!m_headerInfo->MakeData())
        return false;

    return true;
}

bool PacketBufferFix::EvacuateByteStream(const size_t count)
{
    if (m_tempBuffer->Size() >= count)
        return true;

    const size_t reqCount = count - m_tempBuffer->Size();
    const size_t realCount = (m_readSeekpoint + reqCount > m_writeSeekpoint) ? m_writeSeekpoint - m_readSeekpoint : reqCount;

    m_tempBuffer->AppendImpl(&m_buffer[0] + m_readSeekpoint, realCount);
    m_readSeekpoint += realCount;
    return m_tempBuffer->Size() >= count;
}

bool PacketBufferFix::EvacuateChunk(const size_t count)
{
    uint8_t peek = 0;

    while (m_tempBuffer->Size() < count)
    {
        if (!PeekChunk(peek))
            return false;

        if (!m_tempBuffer->Append(peek))     ///WARNING-!???????? ???? ???? ?????? ?????? ???? ????!///
            NET_PUSH_LOGMSG("PacketBufferFix::EvacuateChunk a stream too long!", PrintUtil::ConsoleColor::COLOR_DARKRED);
        ++m_readSeekpoint;
    }
    return true;
}

bool PacketBufferFix::ReadSendInfoDetail()
{
    uint32_t startpoint = 0, endpoint = 0;
    bool delayReturn = true;

    m_tempBuffer->ReadChunk(startpoint);
    m_tempBuffer->ReadChunk(endpoint);
    if (SenderInfo::sender_field_second != endpoint)
    {
        delayReturn = false;
        RewindSeek(sizeof(startpoint) + sizeof(endpoint));      ///???????? read seek point ?????? ?????? ?? ????. ?????? ?????? ????
    }
    else
    {
        socket_type sockId = 0;

        if (m_tempBuffer->ReadChunk(sockId))
            m_senderInfo->m_latestSocketId = sockId;
    }

    return delayReturn;
}

bool PacketBufferFix::ReadSendInfo()
{
    if (!EvacuateChunk(sizeof(SenderInfo)))
        return false;

    m_parseAction = [this]() { return this->ReadStartpoint(); };

    bool res = ReadSendInfoDetail();

    m_tempBuffer->Clear();
    return res;
}

bool PacketBufferFix::ReadPacketDetail()
{
    do
    {
        if (!MakePacketHeaderData())
            break;

        if (!MakePacketImpl())
            break;

        m_packetList.push_back(std::move(m_createdPacket));
        return true;
    }
    while (false);
    NET_PUSH_LOGMSG("PacketBufferFix::ReadPacketDetail error");
    return false;
}

bool PacketBufferFix::ReadPacketEtc()
{
    size_t packetLength = 0;

    m_headerInfo->GetProperty<HeaderData::FieldInfo::LENGTH>(packetLength);

    if (!EvacuateByteStream(packetLength))
        return false;

    uint32_t endpoint = 0;
    bool delayReturn = true;

    m_tempBuffer->Back(endpoint);
    m_parseAction = [this]() { return this->ReadStartpoint(); };    //?????? ???????? ?????? ?????? ???????? ????!
    if (endpoint != HeaderData::header_terminal)        //unmatched!
    {
        RewindSeek(packetLength + sizeof(HeaderData::header_stx));
        delayReturn = false;
        NET_PUSH_LOGMSG("bool PacketBufferFix::ReadPacketEtc() the etx unmatched!", PrintUtil::ConsoleColor::COLOR_DARKRED);
    }
    else
    {
        delayReturn = ReadPacketDetail();
    }
    
    m_tempBuffer->Clear();
    return delayReturn;
}

bool PacketBufferFix::ReadPacketLength()
{
    static constexpr size_t stxlengthSize = sizeof(HeaderData::header_stx) + sizeof(size_t);

    if (!EvacuateChunk(stxlengthSize))
        return false;

    uint32_t length = 0, stx = 0;

    m_tempBuffer->ReadChunk(stx);
    m_tempBuffer->ReadChunk(length);
    m_headerInfo->SetProperty<HeaderData::FieldInfo::LENGTH>(length);
    if (length > NetPacket::packet_unit_max_size)   //too long     //FIXME. require alignment buffer size
    {
        RewindSeek(sizeof(length));
        m_parseAction = [this]() { return this->ReadStartpoint(); };
        m_tempBuffer->Clear();
        NET_PUSH_LOGMSG("bool PacketBufferFix::ReadPacketLength() wrong packet rewind seekpoint..", PrintUtil::ConsoleColor::COLOR_DARKRED);
        return false;
    }
    m_parseAction = [this]() { return this->ReadPacketEtc(); };
    return true;
}

bool PacketBufferFix::ReadStartpoint()
{
    if (!EvacuateChunk(sizeof(uint32_t)))
        return false;

    RewindSeek(sizeof(uint32_t));
    uint32_t magic = 0;

    m_tempBuffer->ReadChunk(magic);
    m_tempBuffer->Clear();
    switch (magic)
    {
    case HeaderData::header_stx:
        m_parseAction = [this]() { return this->ReadPacketLength(); };
        return true;
    case SenderInfo::sender_field_first:    //TODO
        m_parseAction = [this]() { return this->ReadSendInfo(); };
        return true;
    default:
        return false;
    }
}

void PacketBufferFix::ReadStream()
{
    while (m_readSeekpoint < m_writeSeekpoint)
    {
        if (!m_parseAction())
            ++m_readSeekpoint;
    }

    m_readSeekpoint = 0;
    m_writeSeekpoint = 0;
}

bool PacketBufferFix::PushBack(WinSocket *sock, const std::vector<uint8_t> &stream)
{
    if (!AppendSenderInfo(sock))
        return false;

    if (!CheckCapacity(stream.size()))
        return false;

    std::copy(stream.cbegin(), stream.cend(), m_buffer.begin() + m_writeSeekpoint);
    m_writeSeekpoint += stream.size();
    return true;
}

bool PacketBufferFix::IsEmpty() const
{
    return m_packetList.empty() && m_writeSeekpoint == 0;
}

bool PacketBufferFix::PopPacket(std::unique_ptr<NetPacket> &dest)
{
    ReadStream();
    if (m_packetList.size())
    {
        dest = std::move(m_packetList.front());
        m_packetList.pop_front();
        return true;
    }
    return false;
}

size_t PacketBufferFix::DebugPacketBufferSize() const
{
    return m_tempBuffer->Size();
}

