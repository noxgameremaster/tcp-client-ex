
#ifndef FRAME_RATE_THREAD_H__
#define FRAME_RATE_THREAD_H__

#include "netservice.h"

class LoopThread;

class FrameRateThread : public NetService
{
    static constexpr int frame_rate_interval = 30;

private:
    uint32_t m_fps;
    std::unique_ptr<LoopThread> m_frameThread;

private:
    explicit FrameRateThread();
    ~FrameRateThread() override;

private:
    bool GrowingUp();

    void OnInitialOnce() override;
    bool OnInitialize() override;
    void OnDeinitialize() override;

public:
    uint32_t CurrentFps() const
    {
        return m_fps;
    }
    bool IsGreater(const uint32_t currentFps, const uint32_t gap) const;

public:
    static FrameRateThread &FrameThreadObject();
};

#endif

