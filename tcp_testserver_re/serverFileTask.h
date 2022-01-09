
#ifndef SERVER_FILE_TASK_H__
#define SERVER_FILE_TASK_H__

#include "serverTask.h"
#include <vector>

class ServerFileTask : public ServerTask
{
private:
    std::string m_filename;

public:
    explicit ServerFileTask(NetObject *parent = nullptr);
    ~ServerFileTask() override;

private:
    void ServerFileMeta(std::unique_ptr<NetPacket> &&fileMeta);
    void ServerFileStream(std::unique_ptr<NetPacket> &&fileStream);
    void ClientFileRequest(std::unique_ptr<NetPacket> &&req);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;

public:
    void SendFileStream(const std::vector<uint8_t> &stream, const std::string &filename);

    DECLARE_SIGNAL(OnReceiveFileInfo, std::string)
};

#endif

