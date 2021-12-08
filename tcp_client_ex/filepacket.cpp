#include "filepacket.h"
#include "packetordertable.h"
FilePacket::FilePacket()
    : NetPacket()
{
    m_filesize = 0;
    m_filenameLength = 0;
    m_pathLength = 0;
    m_reportError = 0;
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
        return sizeof(m_reportError) + sizeof(m_filenameLength) + m_filename.length();

    return sizeof(m_filesize) + sizeof(m_filenameLength) + sizeof(m_pathLength)
        + m_savepath.length() + m_filename.length();
}

void FilePacket::ReadString(std::string &strDest, const uint8_t &length)
{
    if (length == 0)
        throw false;

    strDest.resize(static_cast<const size_t>(length));
    for (char c : strDest)
        ReadCtx(c);
}

bool FilePacket::OnReadPacket()
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

uint8_t FilePacket::GetPacketId()
{
    return static_cast<uint8_t>(PacketOrderTable<FilePacket>::GetId());
}

bool FilePacket::OnWritePacket()
{
    if (m_filename.length() > 128)
        return false;

    m_filenameLength = static_cast<uint8_t>(m_filename.length());

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



