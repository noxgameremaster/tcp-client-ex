
#include "packetproducer.h"
#include "localbuffer.h"
#include "packetordertable.h"
#include "headerdata.h"
#include "chatpacket.h"
#include "testpacket.h"
#include "echopacket.h"
#include "filepacket.h"
#include "filepacketupload.h"
#include "downloadCompletePacket.h"
#include "filechunkpacket.h"

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

std::unique_ptr<NetPacket> PacketProducer::CreatePacket(const char &packetId)
{
    m_createdPacket.reset();

    switch (packetId)
    {
    case PacketOrderTable<ChatPacket>::GetId(): return std::make_unique<ChatPacket>();
    case PacketOrderTable<TestPacket>::GetId(): return std::make_unique<TestPacket>();
    case PacketOrderTable<EchoPacket>::GetId(): return std::make_unique<EchoPacket>();
    case PacketOrderTable<FilePacket>::GetId(): return std::make_unique<FilePacket>();
    case PacketOrderTable<FileChunkPacket>::GetId(): return std::make_unique<FileChunkPacket>();
    case PacketOrderTable<FilePacketUpload>::GetId(): return std::make_unique<FilePacketUpload>();
    case PacketOrderTable<DownloadCompletePacket>::GetId(): return std::make_unique<DownloadCompletePacket>();
    default: return nullptr;
    }
}

bool PacketProducer::MakePacketImpl(uint32_t offset)
{
    decltype(m_headerdata) headerdata(new HeaderData);

    if (!headerdata->PutStream(m_localbuffer->GetPartStream(offset, headerdata->FieldLength())))
        return false;

    if (!headerdata->MakeData())
        return false;

    m_headerdata = std::move(headerdata);

    uint8_t type = 0;

    m_headerdata->GetProperty<HeaderData::FieldInfo::MAIN_CMD_TYPE>(type);

    {
        auto createdPacket = CreatePacket(type);

        if (!createdPacket)
            return false;       //unknown packet here
        m_createdPacket = std::move(createdPacket);
    }

    size_t length = 0;

    m_headerdata->GetProperty<HeaderData::FieldInfo::LENGTH>(length);

    std::vector<uint8_t> deststream(length, 0);

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

void PacketProducer::Scan()
{
    size_t readpos = 0;
    for (;;)
    {
        int stx = 0;

        if (!m_localbuffer->Peek(stx, readpos)) //eof
            break;

        if (HeaderData::header_stx != stx)
        {
            ++readpos;
            continue;
        }
        m_tempStxposList.push_back(readpos);
        readpos += sizeof(stx);

        if (m_tempStxposList.size() > 10000) //testing
            m_tempStxposList.clear(); //testing

        int length = 0;

        if (!m_localbuffer->PeekInc(length, readpos))
            break;

        int ttx = 0;
        size_t ttxpos = (readpos + length) - sizeof(stx) - sizeof(length) - sizeof(HeaderData::header_terminal);

        if (!m_localbuffer->PeekInc(ttx, ttxpos))
            break;

        if (HeaderData::header_terminal == ttx)
            m_stxposList.push_back(m_tempStxposList.back());
        else
            m_tempStxposList.pop_back();
        readpos = ttxpos;
        m_ttxpos = ttxpos;
    }
}

bool PacketProducer::ReadBuffer()
{
    m_ttxpos = 0;
    m_stxposList.clear();
    Scan();

    do
    {
        if (m_tempStxposList.empty())   //stx 아무것도 못찾은경우
            m_localbuffer->Clear();

        else if (m_stxposList.empty())  //stx 있는데 스트림 부족
        {
            auto &stxpos = m_tempStxposList.front();

            if (stxpos)
            {
                m_localbuffer->Pull(stxpos);  //맨 앞 stx 찾은 데 까지 땡김
                stxpos = 0;
            }
        }

        else
        {
            int popCount = static_cast<int>(m_stxposList.size());

            while ((--popCount) >= 0)
                m_tempStxposList.pop_front();
            break;
        }

        return false;
    }
    while (false);

    return true;
}