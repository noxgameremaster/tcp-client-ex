
#ifndef SERVER_TASK_H__
#define SERVER_TASK_H__

#include "netobject.h"

class NetPacket;

class ServerTask : public NetObject
{
public:
    explicit ServerTask(NetObject *parent = nullptr);
    ~ServerTask() override;

private:
    virtual void DoTask(std::unique_ptr<NetPacket> &&packet) = 0;

protected:
    void ExecuteDoTask(ServerTask *task, std::unique_ptr<NetPacket> &&packet);

private:
    bool ForwardPacketToManager(std::unique_ptr<NetPacket> &&packet, bool toSelf = false);

protected:
    bool ForwardPacket(std::unique_ptr<NetPacket> &&forwardPacket);
    bool SendBackPacket(std::unique_ptr<NetPacket> &&comebackPacket);

public:
    virtual std::string TaskName() const = 0;
};

#endif

