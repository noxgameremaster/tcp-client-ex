
#ifndef SERVER_FILE_TASK_H__
#define SERVER_FILE_TASK_H__

#include "serverTask.h"

class IOFileStream;

class ServerFileTask : public ServerTask
{
private:
    std::unique_ptr<IOFileStream> m_servFile;
    
public:
    explicit ServerFileTask(NetObject *parent = nullptr);
    ~ServerFileTask() override;

private:
    void ServerFileMeta(std::unique_ptr<NetPacket> &&fileMeta);
    void ServerFileStream(std::unique_ptr<NetPacket> &&fileStream);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;
};

#endif

