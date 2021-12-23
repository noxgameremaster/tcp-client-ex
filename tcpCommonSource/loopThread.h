
#ifndef NET_THREAD_H__
#define NET_THREAD_H__

#include "netservice.h"
#include <thread>
#include <functional>
#include <mutex>

class LoopThread : public NetService
{
private:
    std::thread m_thread;
    std::condition_variable m_condvar;
    bool m_terminated;
    std::function<bool()> m_waitCondition;
    std::function<void()> m_waitFunction;
    std::function<void()> m_taskFunction;

public:
    LoopThread(NetObject *parent = nullptr);
    ~LoopThread();

private:
    void WaitCondition();
    void DoTask(std::function<void()> task);
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

public:
    void SetWaitCondition(std::function<bool()> &&cond);
    void SetTaskFunction(std::function<void()> &&task);
    void Notify();

private:
    std::mutex m_waitLock;
};

#endif

