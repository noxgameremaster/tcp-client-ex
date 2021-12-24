
#ifndef FILE_CHUNK_PACKET_H__
#define FILE_CHUNK_PACKET_H__

#include "netpacket.h"
#include <array>

class FileChunkPacket : public NetPacket
{
private:
    std::string m_filename;
    uint16_t m_chunkLength;
    std::array<uint8_t, 65536> m_filechunk;

    //writeOnly
    bool m_isError;
    bool m_isCompleted;
    size_t m_writePos;

public:
    FileChunkPacket();
    ~FileChunkPacket();

private:
    bool OnWritePacket() override;

public:
    static std::string TaskName()
    {
        return "ChunkPacket";
    }

    bool FetchFileChunk(std::vector<uint8_t> &dest);
    void SetReportParam(bool isError, bool isCompleted, const size_t &writeAmount);

private:
    std::string ClassName() const override
    {
        return TaskName();
    }

    bool OnReadPacket() override;
    uint8_t GetPacketId() const override;
};

#endif

