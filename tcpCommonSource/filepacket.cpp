
#include "filepacket.h"
#include "packetordertable.h"

template <FilePacket::FilePacketDirection Direction>
struct PacketDirection;

template <>
struct PacketDirection<FilePacket::FilePacketDirection::ServerToClient>
{
    static constexpr uint8_t Get()
    {
        return 0;
    }
};

template <>
struct PacketDirection<FilePacket::FilePacketDirection::ClientToServer>
{
    static constexpr uint8_t Get()
    {
        return 1;
    }
};

FilePacket::FilePacket()
    : NetPacket()
{
    m_filesize = 0;
    m_filenameLength = 0;
    m_pathLength = 0;
    m_reportError = 0;
    m_downloadBytes = 0;
}

FilePacket::~FilePacket()
{ }

//receive data::
//(byte=filenameLength)+"filename", (byte=pathLength)+"path", (dword=filesize)

//send data::
//(byte=error or not),(byte=filenameLength)+"filename"
size_t FilePacket::PacketSize(Mode mode)
{
    if (mode == Mode::Write)
    {
        switch (GetSubCommand())
        {
        case PacketDirection<FilePacketDirection::ServerToClient>::Get():
            return sizeof(m_filesize) + sizeof(m_filenameLength) + m_filenameLength + sizeof(m_pathLength) + m_pathLength;
        case PacketDirection<FilePacketDirection::ClientToServer>::Get():
            return sizeof(m_reportError) + sizeof(m_filenameLength) + m_filename.length();
        }
    }
    return sizeof(m_filesize) + sizeof(m_filenameLength) + sizeof(m_pathLength)
        + m_savepath.length() + m_filename.length();
}

void FilePacket::ReadString(std::string &strDest, const uint8_t &length)
{
    if (length == 0)
        throw false;

    strDest.resize(static_cast<const size_t>(length));
    for (char &c : strDest)
        ReadCtx(c);
}

bool FilePacket::ReadFromServer()
{
    try
    {
        ReadCtx(m_filenameLength);
        ReadString(m_filename, m_filenameLength);
        ReadCtx(m_pathLength);
        ReadString(m_savepath, m_pathLength);
        ReadCtx(m_filesize);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacket::ReadFromClient()
{
    try
    {
        ReadCtx(m_reportError);
        ReadCtx(m_filenameLength);
        ReadString(m_filename, m_filenameLength);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacket::OnReadPacket()
{
    switch (GetSubCommand())
    {
    case PacketDirection<FilePacketDirection::ServerToClient>::Get(): return ReadFromServer();
    case PacketDirection<FilePacketDirection::ClientToServer>::Get(): return ReadFromClient();
    default: return false;
    }
}

bool FilePacket::CheckStringLimit(const std::string &src, std::string &dest, uint8_t &byteLength)
{
    if (src.length() > 128)
    {
        dest.clear();
        return false;
    }
    dest = src;
    byteLength = static_cast<std::remove_reference<decltype(byteLength)>::type>(src.length());

    return true;
}

uint8_t FilePacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<FilePacket>::GetId());
}

void FilePacket::SetFilePacketDirection(FilePacketDirection dir)
{
    uint8_t subCmd = 0;

    if (FilePacketDirection::ClientToServer == dir)
        subCmd = PacketDirection<FilePacketDirection::ClientToServer>::Get();
    else if (FilePacketDirection::ServerToClient == dir)
        subCmd = PacketDirection<FilePacketDirection::ServerToClient>::Get();

    SetSubCommand(subCmd);
}

bool FilePacket::ClientWrite()
{/*
    SetSubCommand(1);

    std::array<char, 255> data;

    data.fill(0);
    std::copy(m_filename.begin(), m_filename.end(), data.begin());

    try
    {
        for (const auto &c : data)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;*/


    try
    {
        WriteCtx(m_reportError);
        WriteCtx(m_filenameLength);
        for (const char &c : m_filename)
            WriteCtx(c);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacket::ServerWrite()
{
    try
    {
        WriteCtx(m_filenameLength);
        for (const auto &c : m_filename)
            WriteCtx(c);
        WriteCtx(m_pathLength);
        for (const auto &c : m_savepath)
            WriteCtx(c);
        WriteCtx(m_filesize);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FilePacket::OnWritePacket()
{
    switch (GetSubCommand())
    {
    case PacketDirection<FilePacketDirection::ServerToClient>::Get():
        return ServerWrite();
    case PacketDirection<FilePacketDirection::ClientToServer>::Get():
        return ClientWrite();
    default:
        return false;
    }
}



