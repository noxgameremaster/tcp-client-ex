
#ifndef EVENT_THREAD_H__
#define EVENT_THREAD_H__

#include "netservice.h"
#include <future>
#include <atomic>

class EventThread : public NetService
{
private:
    std::condition_variable m_condvar;
    std::function<bool()> m_condition;
    std::function<bool()> m_execution;
    std::future<bool> m_asyncTask;
    std::shared_ptr<std::atomic<bool>> m_halted;

public:
    explicit EventThread(NetObject *parent = nullptr);
    ~EventThread() override;

private:
    void OnInitialOnce() override;
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

    bool Running(std::shared_ptr<std::atomic<bool>> halted);

public:
    void SetCondition(std::function<bool()> &&condition);
    void SetExecution(std::function<bool()> &&execution);
    void SetLocker(std::weak_ptr<std::mutex> lock)
    {
        m_lock = lock;
    }
    void Notify();

private:
    std::weak_ptr<std::mutex> m_lock;
};

#endif

