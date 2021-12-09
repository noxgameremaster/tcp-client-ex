
#ifndef TASK_THREAD_H__
#define TASK_THREAD_H__

#include "abstracttask.h"
#include <thread>
#include <list>
#include <mutex>

class NetPacket;

class TaskThread : public AbstractTask
{
private:
    bool m_terminated;
    std::condition_variable m_condvar;
    std::thread m_taskThread;
    std::list<std::unique_ptr<NetPacket>> m_msglist;

public:
    TaskThread(NetObject *parent);
    ~TaskThread();

private:
    void ExcuteTask(std::unique_ptr<NetPacket> &&msg);
    void Dequeue();
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;
    void DoThreadTask();

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
    std::mutex m_waitLock;
    std::mutex m_lock;
};

#endif

