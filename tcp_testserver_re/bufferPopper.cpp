
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
        ///Todo. ���⿡�� ��Ŷ�� �޾Ҵ�.
        ///���� ���� �Ѱ���� �� ���ΰ�...?
        /// �׳� ���⿡�� ó��
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
