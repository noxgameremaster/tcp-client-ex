
#ifndef TASK_THREAD_H__
#define TASK_THREAD_H__

#include "abstracttask.h"
#include <list>
#include <mutex>

class NetPacket;
class LoopThread;

class TaskThread : public AbstractTask
{
private:
    std::list<std::unique_ptr<NetPacket>> m_msglist;
    std::unique_ptr<LoopThread> m_taskThread;

public:
    TaskThread(NetObject *parent);
    ~TaskThread() override;

private:
    bool IsMessageList() const;
    void ExecuteTask(std::unique_ptr<NetPacket> &&msg);
    bool Dequeue();
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
    void PushBack(std::unique_ptr<NetPacket> &&msg);
    void StopThread();
    void RunThread();

private:
    std::string TaskName() const override
    {
        return "taskThread";
    }

private:
    mutable std::mutex m_lock;
};

#endif

