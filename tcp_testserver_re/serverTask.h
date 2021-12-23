
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
    bool ForwardPacket(std::unique_ptr<NetPacket> &&forwardPacket);

public:
    virtual std::string TaskName() const = 0;
};

#endif

