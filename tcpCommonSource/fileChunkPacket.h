
#ifndef FILE_CHUNK_PACKET_H__
#define FILE_CHUNK_PACKET_H__

#include "netpacket.h"
#include <array>

class FileChunkPacket : public NetPacket
{
public:
    enum class PacketSubCmd
    {
        None,
        PrevToServer,
        PrevToClient,
        SendToServer,
        SendToClient,
        SendEndChunkToClient
    };

private:
    std::string m_filename;
    uint32_t m_chunkLength;
    std::array<uint8_t, 16384> m_filechunk;
    uint8_t m_filenameLength;

    //writeOnly
    bool m_isError;
    bool m_isCompleted;
    size_t m_writePos;

public:
    FileChunkPacket();
    ~FileChunkPacket() override;

private:
    bool ClientWrite();
    bool ServerWrite();
    bool ToFileServerWrite();
    bool OnWritePacket() override;
    size_t PacketSize(Mode mode) override;

public:
    static std::string TaskName()
    {
        return "ChunkPacket";
    }

    bool FetchFileChunk(std::vector<uint8_t> &dest);
    void SetFileChunk(const std::vector<uint8_t> &src);
    void SetFileUrl(const std::string &fileUrl);
    void SetReportParam(bool isError, bool isCompleted, const size_t &writeAmount);
    void GetProgressStatus(bool &err, bool &end, size_t &writePos);

private:
    std::string ClassName() const override
    {
        return TaskName();
    }
    bool ClientRead();
    bool ServerRead();
    bool FileServerRead();

    bool OnReadPacket() override;
    uint8_t GetPacketId() const override;

public:
    void ChangeSubCommand(PacketSubCmd subcmd);
    PacketSubCmd SubCommand() const;
};

#endif
