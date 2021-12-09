
#ifndef TASK_MANAGER_H__
#define TASK_MANAGER_H__

#include "netservice.h"

#include <map>

class TaskThread;
class NetPacket;
class AbstractTask;
class NetFlowControl;
class LargeFile;

class TaskManager : public NetService
{
private:
    std::unique_ptr<TaskThread> m_taskthread;
    std::map<std::string, std::shared_ptr<AbstractTask>> m_taskmap;
    NetFlowControl *m_netFlow;

    std::shared_ptr<LargeFile> m_largefile;

public:
    TaskManager(NetObject *parent);
    ~TaskManager() override;

private:
    bool SetNetFlowIO();
    bool OnInitialize() override;
    bool InsertTask(std::unique_ptr<AbstractTask> &&task);
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

    void SendOnInitial();

public:
    bool GetLargeFileObject(std::weak_ptr<LargeFile> &largefile);
    void InputTask(std::unique_ptr<NetPacket> &&packet);
    AbstractTask *GetTask(const std::string &taskName);
    void ForwardPacket(std::unique_ptr<NetPacket> &&packet);
};

#endif

