
#ifndef TASK_THREAD_H__
#define TASK_THREAD_H__

#include "abstracttask.h"
#include <list>
#include <mutex>

class NetPacket;
//class LoopThread;

class TaskThread : public AbstractTask
{
private:
    std::list<std::unique_ptr<NetPacket>> m_msglist;
    //std::unique_ptr<LoopThread> m_taskThread;
    std::thread m_taskThread;
    bool m_terminated;
    std::condition_variable m_condvar;

public:
    TaskThread(NetObject *parent);
    ~TaskThread() override;

private:
    bool IsMessageList() const;
    void ExecuteTask(std::unique_ptr<NetPacket> &&msg);
    void Dequeue();
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

    std::string ObjectName() const override
    {
        return TaskName();
    }

public:
    int GetCount() const    //Debugging only
    {
        return static_cast<int>(m_msglist.size());
    }

private:
    std::mutex m_lock;
};

#endif

