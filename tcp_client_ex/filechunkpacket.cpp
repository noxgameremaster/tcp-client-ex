
#include "filechunkpacket.h"
#include "packetordertable.h"

FileChunkPacket::FileChunkPacket()
    : NetPacket()
{
    m_chunkLength = 0;
    m_filechunk.fill(0);
}

FileChunkPacket::~FileChunkPacket()
{ }

bool FileChunkPacket::OnWritePacket()
{
    try
    {
        WriteCtx(static_cast<uint8_t>(m_isError));
        WriteCtx(static_cast<uint8_t>(m_isCompleted));
        WriteCtx(m_writePos);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FileChunkPacket::FetchFileChunk(std::vector<uint8_t> &dest)
{
    if (!m_chunkLength)
        return false;

    dest.resize(static_cast<size_t>(m_chunkLength));
    std::copy(m_filechunk.begin(), m_filechunk.end(), dest.begin());
    return true;
}

void FileChunkPacket::SetReportParam(bool isError, bool isCompleted, const size_t &writeAmount)
{
    m_isError = isError;
    m_isCompleted = isCompleted;
    m_writePos = writeAmount;
}

//IN: (byte: filenameLength)+"FILENAME"+ (byte: chunkLength)+ B"Chunk"
//OUT: (byte: isError)+(byte: isCompleted)+(dword: writePos)

bool FileChunkPacket::OnReadPacket()
{
    uint8_t filenameLength = 0;

    try
    {
        ReadCtx(filenameLength);
        m_filename.resize(filenameLength);
        for (auto &c : m_filename)
            ReadCtx(c);
        ReadCtx(m_chunkLength);
        for (decltype(m_chunkLength) u = 0 ; u < m_chunkLength ; ++u)
            ReadCtx(m_filechunk[u]);
    }
    catch (const bool &fail)
    {
        return fail;
    }

    return true;
}

uint8_t FileChunkPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<FileChunkPacket>::GetId());
}

