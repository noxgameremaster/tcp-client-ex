
#include "packetproducer.h"
#include "localbuffer.h"
#include "packetordertable.h"
#include "headerdata.h"
#include "chatpacket.h"
//#include "testpacket.h"
#include "echopacket.h"

PacketProducer::PacketProducer()
    : NetObject()
{
    m_ttxpos = 0;
}

PacketProducer::~PacketProducer()
{ }

bool PacketProducer::PacketPostProc()
{
    if (!m_createdPacket)
        return false;

    m_createdPacket->SetHeaderData(std::move(m_headerdata));

    bool bReadok = m_createdPacket->Read();

    if (bReadok)
        m_createdPacket->DoAction();

    return bReadok;
}

bool PacketProducer::CreatePacket(const char &packetId)
{
    m_createdPacket.reset();

    do
    {
        if (packetId == PacketOrderTable<ChatPacket>::GetId())
            m_createdPacket = std::make_unique<ChatPacket>();
        /*else if (packetId == PacketOrderTable<TestPacket>::GetId())
            m_createdPacket = std::make_unique<TestPacket>();*/
        else if (packetId == PacketOrderTable<EchoPacket>::GetId())
            m_createdPacket = std::make_unique<EchoPacket>();
        else
            return false;
    }
    while (false);

    return true;
}

uint32_t PacketProducer::CheckValidPacket()
{
    uint32_t readpos = 0, cstxpos = static_cast<uint32_t>(-1);
    int stx = 0;

    for (;;)
    {
        if (!m_localbuffer->Peek(stx, readpos))     //EOF
            break;

        if (packet_header_stx != stx)
        {
            ++readpos;
            continue;
        }

        cstxpos = readpos;
        int length = 0;

        readpos += sizeof(stx);
        if (!m_localbuffer->PeekInc(length, readpos))
            break;

        int ttx = 0;
        uint32_t ttxpos = (readpos + length) - sizeof(stx) - sizeof(length) - sizeof(packet_header_ttx);

        if (!m_localbuffer->PeekInc(ttx, ttxpos))
            break;

        if (packet_header_ttx != ttx)
        {
            if (m_stxposList.empty())
                cstxpos = static_cast<decltype(cstxpos)>(-1);
            break;
        }

        m_stxposList.push_back(cstxpos);
        m_ttxpos = ttxpos;
        readpos = ttxpos;
    }
    return cstxpos;
}

bool PacketProducer::MakePacketImpl(uint32_t offset)
{
    decltype(m_headerdata) headerdata(new HeaderData);

    if (!headerdata->MakeData(m_localbuffer, offset))
        return false;

    m_headerdata = std::move(headerdata);

    uint8_t type = 0;

    m_headerdata->GetProperty<HeaderData::FieldInfo::TYPE>(type);

    if (!CreatePacket(type))
        return false;       //unknown packet here

    int length = 0;

    m_headerdata->GetProperty<HeaderData::FieldInfo::LENGTH>(length);

    std::vector<uint8_t> deststream(static_cast<size_t>(length), 0);

    if (!m_localbuffer->PopN(deststream, offset))
        return false;

    m_createdPacket->PutStream(deststream);

    return PacketPostProc();
}

void PacketProducer::SetCapture(NetObject *target, capture_function &&notifier)
{
    m_target.reset();
    if (target == nullptr)
        return;
    GetImpl(target, m_target);
    if (m_target.expired())
        return;
    m_notifier = notifier;
}

void PacketProducer::MakePacket()
{
    bool intercept = !m_target.expired();

    for (const auto &offset : m_stxposList)
    {
        MakePacketImpl(offset);
        //TODO. 여기에서 패킷을 뜯던지 해야함.
        if (intercept)
            m_notifier(std::move(m_createdPacket));
    }
    if (m_ttxpos)
        m_localbuffer->Pull(m_ttxpos);
}

void PacketProducer::SetLocalBuffer(std::shared_ptr<LocalBuffer> buffer)
{
    m_localbuffer = buffer;
}

bool PacketProducer::ReadBuffer()
{
    m_stxposList.clear();
    uint32_t pos = CheckValidPacket();

    do
    {
        if (pos == -1)
            m_localbuffer->Clear();
        else if (m_stxposList.empty())
            m_localbuffer->Pull(pos);
        else
            break;

        return false;
    }
    while (false);

    return true;
}

