
#ifndef ABSTRACT_TASK_H__
#define ABSTRACT_TASK_H__

#include "netobject.h"

class NetPacket;

class AbstractTask : public NetObject
{
public:
    AbstractTask(NetObject *parent = nullptr);
    ~AbstractTask() override;

private:
    virtual void DoTask(std::unique_ptr<NetPacket> &&packet) = 0;

protected:
    void ExecuteDoTask(AbstractTask *task, std::unique_ptr<NetPacket> &&packet);
    bool ForwardPacketToManager(std::unique_ptr<NetPacket> &&forwardPacket, bool toInner = false);

public:
    virtual std::string TaskName() const = 0;
};

#endif

