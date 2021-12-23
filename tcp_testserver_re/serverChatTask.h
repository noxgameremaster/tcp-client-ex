
#ifndef SERVER_CHAT_TASK_H__
#define SERVER_CHAT_TASK_H__

#include "serverTask.h"

class ServerChatTask : public ServerTask
{
public:
    explicit ServerChatTask(NetObject *parent = nullptr);
    ~ServerChatTask() override;

private:
    bool CheckValidColor(uint8_t colrbyte);
    void ShowAll(const std::string &message);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;

private:
    std::string TaskName() const override;
};

#endif

