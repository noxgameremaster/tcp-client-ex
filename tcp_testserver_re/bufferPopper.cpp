
#include "bufferPopper.h"
#include "loopThread.h"
#include "simpleBuffer.h"
#include "netpacket.h"

BufferPopper::BufferPopper()
    : NetService()
{
    m_popThread = std::make_unique<LoopThread>();
    m_popThread->SetTaskFunction([this]() { this->PopData(); });
}

BufferPopper::~BufferPopper()
{ }

void BufferPopper::PopData()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    if (!m_simpbuffer)
        return;
    if (m_simpbuffer->IsEmpty())
        return;

    std::unique_ptr<NetPacket> getPacket;

    while (m_simpbuffer->PopPacket(getPacket))
    {
        ///Todo. 여기에서 패킷을 받았다.
        ///이제 어디로 넘겨줘야 할 것인가...?
        /// 그냥 여기에서 처리
        m_OnReleasePacket.Emit(std::move(getPacket));
    }
}

bool BufferPopper::OnInitialize()
{
    return true;
}

bool BufferPopper::OnStarted()
{
    return m_popThread->Startup();
}

void BufferPopper::OnDeinitialize()
{ }

void BufferPopper::OnStopped()
{
    m_popThread->Shutdown();
}

void BufferPopper::SlotRegistBuffer(std::shared_ptr<SimpleBuffer> buffer)
{
    if (buffer)
        m_simpbuffer = buffer;
}
