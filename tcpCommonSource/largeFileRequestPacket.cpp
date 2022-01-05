
#include "largeFileRequestPacket.h"
#include "packetordertable.h"

LargeFileRequestPacket::LargeFileRequestPacket()
    : NetPacket()
{
    m_fileUrlLength = 0;
    m_requestUrlArray.fill(0);
}

LargeFileRequestPacket::~LargeFileRequestPacket()
{ }

size_t LargeFileRequestPacket::PacketSize(Mode mode)
{
    if (Mode::Write != mode)
        return 0;

    switch (SubCommand())
    {
    case PacketSubCmd::StartTestToServer:
        return sizeof(m_fileUrlLength) + m_fileUrlLength;
    case PacketSubCmd::SendToServer:
        return m_requestUrlArray.max_size();
    default: return 0;
    }
}

bool LargeFileRequestPacket::ReadStartTest()
{
    try
    {
        ReadCtx(m_fileUrlLength);
        m_requestFileUrl.resize(static_cast<decltype(m_fileUrlLength)>(m_fileUrlLength));
        for (auto &c : m_requestFileUrl)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileRequestPacket::ReadRequestUrlToServer()
{
    try
    {
        for (auto &c : m_requestUrlArray)
            ReadCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileRequestPacket::OnReadPacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::StartTestToServer: return ReadStartTest();
    case PacketSubCmd::SendToClient: return true;
    case PacketSubCmd::SendToServer: return true;
    default: return false;
    }
}

bool LargeFileRequestPacket::WriteStartTest()
{
    if (m_requestFileUrl.empty())
        return false;

    if (m_fileUrlLength != static_cast<decltype(m_fileUrlLength)>(m_requestFileUrl.length()))
        m_requestFileUrl.resize(static_cast<size_t>(m_fileUrlLength));
    try
    {
        WriteCtx(m_fileUrlLength);
        for (const auto &c : m_requestFileUrl)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileRequestPacket::WriteRequestUrlToServer()
{
    try
    {
        for (const auto &c : m_requestUrlArray)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileRequestPacket::OnWritePacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::SendToServer: return WriteRequestUrlToServer();
    case PacketSubCmd::StartTestToServer: return WriteStartTest();
    default: return false;
    }
}

uint8_t LargeFileRequestPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<LargeFileRequestPacket>::GetId());
}

void LargeFileRequestPacket::ChangeSubCommand(PacketSubCmd subcmd)
{
    auto setter = [](PacketSubCmd sub)->uint8_t
    {
        switch (sub)
        {
        case PacketSubCmd::StartTestToServer: return 101;
        case PacketSubCmd::SendToServer: return 1;
        case PacketSubCmd::SendToClient: return 2;
        default: return 0;
        }
    };

    SetSubCommand(setter(subcmd));
}

LargeFileRequestPacket::PacketSubCmd LargeFileRequestPacket::SubCommand() const
{
    switch (GetSubCommand())
    {
    case 1: return PacketSubCmd::SendToServer;
    case 2: return PacketSubCmd::SendToClient;
    case 101: return PacketSubCmd::StartTestToServer;
    default: return PacketSubCmd::None;
    }
}

void LargeFileRequestPacket::SetRequestFileUrl(const std::string &requestFileUrl)
{
    m_requestFileUrl = requestFileUrl;

    if (requestFileUrl.length() > 128)
        m_requestFileUrl.resize(128, 0);

    m_fileUrlLength = static_cast<decltype(m_fileUrlLength)>(m_requestFileUrl.length());

    m_requestUrlArray.fill(0);
    std::copy(m_requestFileUrl.cbegin(), m_requestFileUrl.cend(), m_requestUrlArray.begin());
}