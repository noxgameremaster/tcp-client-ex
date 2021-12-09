
#include "filechunkpacket.h"
#include "packetordertable.h"

FileChunkPacket::FileChunkPacket()
    : NetPacket()
{
    m_filechunk.fill(0);
}

FileChunkPacket::~FileChunkPacket()
{ }


//IN: (byte: filenameLength)+"FILENAME"+ (byte: chunkLength)+ B"Chunk"
//OUT: (

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

