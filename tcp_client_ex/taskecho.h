
#ifndef TASK_ECHO_H__
#define TASK_ECHO_H__

#include "abstracttask.h"

class NetPacket;

class TaskEcho : public AbstractTask
{
public:
    TaskEcho();
    ~TaskEcho();

private:
    void PrintEchoMessage(const std::string &echoMessage) const;
    virtual void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
    static std::string TaskName();
};

#endif

