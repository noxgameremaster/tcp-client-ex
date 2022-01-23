
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
    std::shared_ptr<std::atomic<bool>> m_halted;
    task_function_type m_taskFunction;

public:
    explicit LoopThread(NetObject *owner = nullptr);
    ~LoopThread() override;

private:
    void ThreadHalted();
    void DoTask(task_function_type task, std::shared_ptr<std::atomic<bool>> halted);
    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

public:
    void SetTaskFunction(task_function_type &&task);
};

#endif

