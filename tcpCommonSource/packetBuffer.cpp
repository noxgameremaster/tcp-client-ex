
#include "packetBuffer.h"
#include "winsocket.h"
#include "netpacket.h"
#include "headerdata.h"
#include <assert.h>

PacketBuffer::PacketBuffer()
    : BinaryStream(1)
{
    m_latestSocketId = WinSocket::invalid_socket;
    m_buffer.resize(max_buffer_length);
    SetContext(&m_buffer);
    ResetSeekPoint();
}

PacketBuffer::~PacketBuffer()
{ }

bool PacketBuffer::ResetSeekPoint()
{
    m_readSeekpoint = 0;
    m_writeSeekpoint = 0;

    return true;
}

bool PacketBuffer::CheckCapacity(const size_t &inputSize)
{
    return (m_writeSeekpoint + inputSize) <= max_buffer_length;
}

bool PacketBuffer::AppendSenderInfo(WinSocket *sock)
{
    socket_type sockId = sock->GetFd();

    /*if (sockId == -1)
        return true;*/

    if (m_latestSocketId == sockId)
        return true;

    if (!CheckCapacity(sizeof(sockId) * 3))
        return false;

    WriteChunk(sender_field_front);
    WriteChunk(sender_field_back);
    WriteChunk(sockId);
    return true;
}

bool PacketBuffer::PushBack(WinSocket *sock, const std::vector<uint8_t> &stream)
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        if (!AppendSenderInfo(sock))
            return false;

        if (!CheckCapacity(stream.size()))
            return false;

        std::copy(stream.cbegin(), stream.cend(), m_buffer.begin() + m_writeSeekpoint);
        m_writeSeekpoint += stream.size();
    }
    return true;
}

//마지막 읽은 위치로 부터 끝까지를 임시버퍼에??
bool PacketBuffer::Pulling(size_t off)
{
    m_readSeekpoint = 0;
    if (!m_writeSeekpoint)
        return false;
    if (!off)
        return false;
    if (off >= m_writeSeekpoint)
        return false;

    std::copy(m_buffer.cbegin() + off, m_buffer.cbegin() + m_writeSeekpoint, m_buffer.begin());
    m_writeSeekpoint -= off;
    return true;
}

std::unique_ptr<NetPacket> PacketBuffer::PacketInstance()
{
    uint8_t packetId = 0;

    if (!m_headerData->GetProperty<HeaderData::FieldInfo::MAIN_CMD_TYPE>(packetId))
        return nullptr;

    return m_instanceFunction ? m_instanceFunction(packetId) : nullptr;
}

bool PacketBuffer::MakePacketReal(const size_t &off)
{
    auto pack = PacketInstance();

    if (!pack)
        return false;

    m_createdPacket = std::move(pack);

    size_t length = 0;

    if (!m_headerData->GetProperty<HeaderData::FieldInfo::LENGTH>(length))
        return false;

    std::vector<uint8_t> stream(length);
    size_t endpos = off + length;

    m_createdPacket->SetSenderSocketId(m_latestSocketId);
    m_createdPacket->SetHeaderData(std::move(m_headerData));
    m_createdPacket->PutStream(std::vector<uint8_t>(m_buffer.cbegin() + off, m_buffer.cbegin() + endpos));

    return m_createdPacket->Read();
}

bool PacketBuffer::MakePacketHeaderData(const size_t &startOff/*, const size_t &length*/)
{
    std::unique_ptr<HeaderData> header(new HeaderData);
    size_t endpos = startOff + header->FieldLength();

    if (!header->PutStream(std::vector<uint8_t>(m_buffer.cbegin() + startOff, m_buffer.cbegin() + endpos)))
        return false;

    if (!header->MakeData())
        return false;

    m_headerData = std::move(header);

    return true;
}

bool PacketBuffer::ReadSenderInfo()
{
    int sub = 0;
    socket_type sockId = 0;

    if (!ReadChunk(sub))
        return false;

    if (!ReadChunk(sockId))
        return false;

    if (sub == sender_field_back)
        m_latestSocketId = sockId;
    return true;
}

bool PacketBuffer::ReadPacketInfo()
{
    size_t length = 0;
    size_t stxOff = m_readSeekpoint - sizeof(length);

    if (!ReadChunk(length))
        return false;

    if (length >= 32768)
    {
        assert(false);  //length too long!
        return true;
    }

    //m_readSeekpoint += (length - sizeof(length));   //exclude stx
    m_readSeekpoint += (length - (sizeof(length) * 3));  //include stx

    int endSymbol = 0;

    if (!ReadChunk(endSymbol))
        return false;

    if (endSymbol != HeaderData::header_terminal)
        return true;

    if (MakePacketHeaderData(stxOff/*, length*/))
    {
        if (MakePacketReal(stxOff))
            m_packetList.push_back(std::move(m_createdPacket));
    }
    return true;
}

//패킷 채로 준다?
bool PacketBuffer::PopAsPacket()
{
    while (m_readSeekpoint < m_writeSeekpoint)
    {
        uint32_t readc = 0;

        if (!PeekChunk(readc))
            return Pulling(m_readSeekpoint);

        size_t readcPos = m_readSeekpoint;

        switch (readc)
        {
        case sender_field_front:
            m_readSeekpoint += sizeof(readc);
            if (!ReadSenderInfo())
                return Pulling(readcPos);
            break;

        case HeaderData::header_stx:
            m_readSeekpoint += sizeof(readc);
            if (!ReadPacketInfo())
                return Pulling(readcPos);
            break;

        default:
            ++m_readSeekpoint;
        }
    }
    return ResetSeekPoint();
}

bool PacketBuffer::IsEmpty() const
{
    std::lock_guard<std::mutex> guard(m_lock);

    return m_writeSeekpoint == 0;
}

bool PacketBuffer::PopPacket(std::unique_ptr<NetPacket> &dest)
{
    std::lock_guard<std::mutex> guard(m_lock);

    PopAsPacket();

    if (m_packetList.size())
    {
        dest = std::move(m_packetList.front());
        m_packetList.pop_front();
        return true;
    }
    return false;
}
