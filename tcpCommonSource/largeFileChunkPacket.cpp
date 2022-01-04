
#include "largeFileChunkPacket.h"
#include "packetOrderTable.h"

LargeFileChunkPacket::LargeFileChunkPacket()
    : NetPacket()
{
    m_streamLength = 0;
}

LargeFileChunkPacket::~LargeFileChunkPacket()
{ }

bool LargeFileChunkPacket::OnWritePacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::SendToServer: return true;
    default: return false;
    }
}

size_t LargeFileChunkPacket::PacketSize(Mode mode)
{
    if (Mode::Write != mode)
        return 0;

    return 0;
}

bool LargeFileChunkPacket::ReadStreamFromServer()
{
    decltype(m_streamLength) counter = 0;
    try
    {
        ReadCtx(m_streamLength);
        while (counter < m_streamLength)
            ReadCtx(m_fileStream[counter++]);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool LargeFileChunkPacket::OnReadPacket()
{
    switch (SubCommand())
    {
    case PacketSubCmd::SendToServer: return ReadStreamFromServer();
    default: return false;
    }
}

uint8_t LargeFileChunkPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<LargeFileChunkPacket>::GetId());
}

void LargeFileChunkPacket::ChangeSubCommand(PacketSubCmd subcmd)
{
    auto getter = [](PacketSubCmd sub)->uint8_t
    {
        switch (sub)
        {
        case PacketSubCmd::SendToServer: return 1;
        case PacketSubCmd::SendToClient: return 2;
        case PacketSubCmd::SentLastDataToClient: return 4;
        default: return 0;
        }
    };
    SetSubCommand(getter(subcmd));
}

LargeFileChunkPacket::PacketSubCmd LargeFileChunkPacket::SubCommand() const
{
    switch (GetSubCommand())
    {
    case 1: return PacketSubCmd::SendToServer;
    case 2: return PacketSubCmd::SendToClient;
    case 4: return PacketSubCmd::SentLastDataToClient;
    default: return PacketSubCmd::None;
    }
}

bool LargeFileChunkPacket::GetFileStream(std::vector<uint8_t> &dest)
{
    if (!m_streamLength)
        return false;

    dest.resize(m_streamLength);

    std::copy_n(m_fileStream.cbegin(), m_streamLength, dest.begin());
    return true;
}
