
#ifndef TASK_MANAGER_H__
#define TASK_MANAGER_H__

#include "netservice.h"

#include <map>

class TaskThread;
class IOBuffer;
class NetPacket;
class AbstractTask;

class TaskManager : public NetService
{
private:
    std::unique_ptr<TaskThread> m_taskthread;
    std::weak_ptr<IOBuffer> m_sendbuffer;
    std::map<std::string, std::shared_ptr<AbstractTask>> m_taskmap;

public:
    TaskManager(NetObject *parent);
    ~TaskManager() override;

private:
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

public:
    void SetSendBuffer(std::shared_ptr<IOBuffer> sendbuffer);
    void InputTask(std::unique_ptr<NetPacket> &&packet);
    AbstractTask *GetTask(const std::string &taskName);
    void SendPacket(std::unique_ptr<NetPacket> &&packet);
};

#endif

