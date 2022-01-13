
#include "frameRateThread.h"
#include "loopThread.h"

FrameRateThread::FrameRateThread()
    : NetService()
{
    m_fps = 0;
}

FrameRateThread::~FrameRateThread()
{ }

bool FrameRateThread::GrowingUp()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(frame_rate_interval));

    ++m_fps;
    return true;
}

void FrameRateThread::OnInitialOnce()
{
    m_frameThread = std::make_unique<LoopThread>(this);

    m_frameThread->SetTaskFunction([this]() { return this->GrowingUp(); });
}

bool FrameRateThread::OnInitialize()
{
    return m_frameThread->Startup();
}

void FrameRateThread::OnDeinitialize()
{
    m_frameThread->Shutdown();
}

bool FrameRateThread::IsGreater(const uint32_t currentFps, const uint32_t gap) const
{
    const uint32_t sub = (m_fps > currentFps) ? m_fps - currentFps : currentFps - m_fps;

    return gap <= sub;
}

FrameRateThread &FrameRateThread::FrameThreadObject()
{
    static FrameRateThread threadObject;

    return threadObject;
}

