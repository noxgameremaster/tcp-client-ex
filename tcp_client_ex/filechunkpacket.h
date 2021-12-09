
#ifndef FILE_CHUNK_PACKET_H__
#define FILE_CHUNK_PACKET_H__

#include "netpacket.h"
#include <array>

class FileChunkPacket : public NetPacket
{
private:
    std::string m_filename;
    uint8_t m_chunkLength;
    std::array<char, 256> m_filechunk;

public:
    FileChunkPacket();
    ~FileChunkPacket();

private:
    std::string ClassName() const override
    {
        return "ChunkPacket";
    }
    bool OnReadPacket() override;
    uint8_t GetPacketId() const override;
};

#endif

