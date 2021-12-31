
#ifndef FILE_PACKET_UPLOAD_H__
#define FILE_PACKET_UPLOAD_H__

#include "netpacket.h"

class FilePacketUpload : public NetPacket
{
public:
    enum class PacketSubCmd
    {
        None,
        TestSendToServer,
        SendToServer,
        SendToClient,
        FileServerToClient,
        ToFileServer
    };

private:
    std::string m_uploadPath;
    uint8_t m_pathLength;

public:
    FilePacketUpload();
    ~FilePacketUpload() override;

private:
    size_t PacketSize(Mode mode) override;

public:
    static std::string TaskName()
    {
        return "FilePacketUpload";
    }

private:
    bool ReadSendTesting();
    bool ReadFromFileServer();
    bool OnReadPacket() override;

    bool TestSendWrite();
    bool ToFileServerWrite();
    bool OnWritePacket() override;
    uint8_t GetPacketId() const override;

    std::string ClassName() const override
    {
        return TaskName();
    }

public:
    void SetUploadPath(const std::string &upPath);
    std::string UploadPath() const
    {
        return m_uploadPath;
    }
    void ChangeSubCommand(PacketSubCmd subcmd);
    PacketSubCmd SubCommand() const;
};

#endif

