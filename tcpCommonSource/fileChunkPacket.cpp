
#include "filechunkpacket.h"
#include "packetordertable.h"

FileChunkPacket::FileChunkPacket()
    : NetPacket()
{
    m_chunkLength = 0;
    m_filechunk.fill(0);
    m_isCompleted = false;
    m_isError = false;
    m_writePos = 0;
    m_filenameLength = 0;
}

FileChunkPacket::~FileChunkPacket()
{ }

bool FileChunkPacket::ClientWrite()
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

bool FileChunkPacket::ServerWrite()
{
    decltype(m_chunkLength) chunkCount = 0;

    try
    {
        WriteCtx(m_filenameLength);
        for (const auto &c : m_filename)
            WriteCtx(c);
        WriteCtx(m_chunkLength);
        while (chunkCount < m_chunkLength)
            WriteCtx(m_filechunk[chunkCount++]);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    return true;
}

bool FileChunkPacket::OnWritePacket()
{
    switch (GetSubCommand())
    {
    case 1:
        return ClientWrite();

    case 0:
        return ServerWrite();

    default:
        return false;
    }
}

size_t FileChunkPacket::PacketSize(Mode mode)
{
    if (mode == Mode::Write)
    {
        switch (GetSubCommand())
        {
        case 1:
            return sizeof(m_isError) + sizeof(m_isCompleted) + sizeof(m_writePos);
        case 0:
            return sizeof(m_filenameLength) + m_filenameLength + sizeof(m_chunkLength) + m_chunkLength;
        }
    }
    return 0;
}

bool FileChunkPacket::FetchFileChunk(std::vector<uint8_t> &dest)
{
    if (!m_chunkLength)
        return false;

    dest.resize(static_cast<size_t>(m_chunkLength));

    size_t writeOffset = 0;

    for (auto &uc : dest)
        uc = m_filechunk[writeOffset++];
    return true;
}

void FileChunkPacket::SetFileChunk(const std::vector<uint8_t> &src)
{
    if (src.size() > m_filechunk.max_size())
        return;

    std::copy(src.cbegin(), src.cend(), m_filechunk.begin());
    m_chunkLength = static_cast<decltype(m_chunkLength)>(src.size());
}

void FileChunkPacket::SetFileUrl(const std::string &fileUrl)
{
    static constexpr size_t filename_length_limit = sizeof(m_filenameLength) << 8;

    if (fileUrl.length() > filename_length_limit)
    {
        m_filename.resize(filename_length_limit / 2);
        std::copy_n(fileUrl.cbegin(), m_filename.size(), m_filename.begin());
    }
    else
        m_filename = fileUrl;
    m_filenameLength = static_cast<decltype(m_filenameLength)>(m_filename.length());
}

void FileChunkPacket::SetReportParam(bool isError, bool isCompleted, const size_t &writeAmount)
{
    m_isError = isError;
    m_isCompleted = isCompleted;
    m_writePos = writeAmount;
}

void FileChunkPacket::SetSubCommand(uint8_t subCmd)
{
    NetPacket::SetSubCommand(subCmd);
}

void FileChunkPacket::GetProgressStatus(bool &err, bool &end, size_t &writePos)
{
    err = m_isError;
    end = m_isCompleted;
    writePos = m_writePos;
}

//IN: (byte: filenameLength)+"FILENAME"+ (word: chunkLength)+ B"Chunk"
//OUT: (byte: isError)+(byte: isCompleted)+(dword: writePos)

bool FileChunkPacket::ClientRead()
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

bool FileChunkPacket::ServerRead()
{
    uint8_t isError = 0, isComplete = 0;

    try
    {
        ReadCtx(isError);
        ReadCtx(isComplete);
        ReadCtx(m_writePos);
    }
    catch (const bool &fail)
    {
        return fail;
    }
    m_isCompleted = isComplete ? true : false;
    m_isError = isError ? true : false;
    return true;
}

bool FileChunkPacket::OnReadPacket()
{
    switch (GetSubCommand())
    {
    case 0:
        return ClientRead();
    case 1:
        return ServerRead();
    default:
        return false;
    }
}

uint8_t FileChunkPacket::GetPacketId() const
{
    return static_cast<uint8_t>(PacketOrderTable<FileChunkPacket>::GetId());
}

