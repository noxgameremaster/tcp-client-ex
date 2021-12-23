
#ifndef SERVER_ECHO_TASK_H__
#define SERVER_ECHO_TASK_H__

#include "serverTask.h"

class NetPacket;

class ServerEchoTask : public ServerTask
{
public:
    ServerEchoTask(NetObject *parent = nullptr);
    ~ServerEchoTask() override;

private:
    void SendEchoResponse(const socket_type &senderId, const std::string &echoMsg);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    std::string TaskName() const override;
};

#endif

