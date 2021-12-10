
#ifndef TASK_ECHO_H__
#define TASK_ECHO_H__

#include "abstracttask.h"

class NetPacket;

class TaskEcho : public AbstractTask
{
public:
    TaskEcho(NetObject *parent);
    ~TaskEcho();

private:
    void ReversalEchoPacket();
    void PrintEchoMessage(const std::string &echoMessage) const;
    virtual void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
    std::string TaskName() const override;
};

#endif
