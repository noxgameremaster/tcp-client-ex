
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

public:
    FilePacket();
    ~FilePacket();

private:
    size_t PacketSize(Mode mode) override;
    void ReadString(std::string &strDest, const uint8_t &length);
    bool OnReadPacket() override;

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
        m_filename = filename;
    }

    std::string GetFilePath() const
    {
        return m_savepath;
    }

    size_t GetFilesize() const
    {
        return static_cast<size_t>(m_filesize);
    }

    uint8_t GetPacketId() override;

    void SetError(const uint8_t &errorId)
    {
        m_reportError = errorId;
    }

private:
    std::string ClassName() const override
    {
        return TaskName();
    }
    bool OnWritePacket() override;

};

#endif

