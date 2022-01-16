
#ifndef NET_THREAD_H__
#define NET_THREAD_H__

#include "netservice.h"
#include <thread>
#include <functional>
#include <mutex>

class LoopThread : public NetService
{
    using task_function_type = std::function<bool()>;

private:
    std::thread m_thread;
    std::condition_variable m_condvar;
    bool m_terminated;
    std::function<bool()> m_waitCondition;
    std::function<void()> m_waitFunction;
    task_function_type m_taskFunction;

    bool m_useCondition;

public:
    explicit LoopThread(NetObject *owner = nullptr);
    ~LoopThread() override;

private:
    void WaitCondition();
    void ThreadHalted();
    void DoTask(task_function_type task);
    void DoTaskCondition(task_function_type task);
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

public:
    void SetWaitCondition(std::function<bool()> &&cond);
    void SetTaskFunction(task_function_type &&task);
    void Notify();

private:
    std::mutex m_waitLock;
};

#endif

