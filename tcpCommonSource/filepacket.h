
#ifndef FILE_PACKET_H__
#define FILE_PACKET_H__

#include "netpacket.h"

class FilePacket : public NetPacket
{
private:
    std::string m_filename;
    std::string m_savepath;
    uint32_t m_filesize;
    uint8_t m_filenameLength;
    uint8_t m_pathLength;
    uint8_t m_reportError;

    size_t m_downloadBytes;
    bool m_completed;

public:
    FilePacket();
    ~FilePacket() override;

private:
    size_t PacketSize(Mode mode) override;
    void ReadString(std::string &strDest, const uint8_t &length);
    bool ReadFromServer();
    bool ReadFromClient();
    bool OnReadPacket() override;

    bool CheckStringLimit(const std::string &src, std::string &dest, uint8_t &byteLength);

public:
    static std::string TaskName()
    {
        return "FilePacket";
    }

    std::string GetFileName() const
    {
        return m_filename;
    }

    void SetFileName(const std::string &filename)
    {
        CheckStringLimit(filename, m_filename, m_filenameLength);
    }
    void SetSavePath(const std::string &path)
    {
        CheckStringLimit(path, m_savepath, m_pathLength);
    }

    std::string GetFilePath() const
    {
        return m_savepath;
    }

    void SetFilesize(const size_t &filesize)
    {
        m_filesize = filesize;
    }

    size_t GetFilesize() const
    {
        return static_cast<size_t>(m_filesize);
    }

    void SetDownloadBytes(const size_t bytes)
    {
        m_downloadBytes = bytes;
    }

    size_t GetDownloadBytes() const
    {
        return m_downloadBytes;
    }

    uint8_t GetPacketId() const override;

    void SetError(const uint8_t &errorId)
    {
        m_reportError = errorId;
    }

public:
    enum class FilePacketDirection
    {
        ClientToServer,
        ServerToClient
    };
    void SetFilePacketDirection(FilePacketDirection dir);

private:
    std::string ClassName() const override
    {
        return TaskName();
    }

    bool ClientWrite();
    bool ServerWrite();
    bool OnWritePacket() override;

public:
    void SetComplete()
    {
        m_completed = true;
    }
    bool IsCompleted() const
    {
        return m_completed;
    }
};

#endif

