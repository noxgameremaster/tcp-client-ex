
#include "clientworker.h"
#include "netflowcontrol.h"
#include "netclient.h"
#include "iobuffer.h"
#include "localbuffer.h"
#include "packetproducer.h"

ClientWorker::ClientWorker(NetObject *parent)
    : NetService(parent)
{
    m_terminated = false;
}

ClientWorker::~ClientWorker()
{ }

void ClientWorker::InterceptPacket(std::unique_ptr<NetPacket> &&pack)
{
    ///생성된 패킷은 여기로옴. 안받으면 일방적인 파기
    if (m_flowcontrol.expired())
        return;

    std::shared_ptr<NetFlowControl> flowcontrol = m_flowcontrol.lock();

    flowcontrol->ReceivePacket(std::move(pack));
}

bool ClientWorker::FetchFromBuffer()
{
    if (m_localbuffer->IsEmpty())
        return false;

    if (m_analyzer->ReadBuffer())
        m_analyzer->MakePacket();

    return true;
}

bool ClientWorker::DoTask()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> waitlock(m_waitlock);
            m_condvar.wait(waitlock);
        }
        if (m_terminated)
            break;

        m_recvbuffer->MoveBuffer(m_localbuffer);

        while (FetchFromBuffer());
    }
    return true;
}

void ClientWorker::BufferOnPushed()
{
    m_condvar.notify_one();
}

void ClientWorker::SetReceiveBuffer(std::shared_ptr<IOBuffer> recvBuffer)
{
    m_recvbuffer = recvBuffer;
}

bool ClientWorker::OnInitialize()
{
    if (!m_recvbuffer)
        return false;

    m_analyzer = std::make_unique<PacketProducer>();
    m_localbuffer = std::make_shared<LocalBuffer>();
    m_analyzer->SetLocalBuffer(m_localbuffer);

    m_recvbuffer->SetTrigger(this, std::bind(&ClientWorker::BufferOnPushed, this));

    NetObject *parent = GetParent();

    if (parent != nullptr)
    {
        NetClient *client = dynamic_cast<NetClient *>(parent);

        if (client != nullptr)
            m_flowcontrol = client->FlowControl();
    }

    return true;
}

void ClientWorker::OnDeinitialize()
{
    if (m_recvbuffer)
        m_recvbuffer.reset();
}

bool ClientWorker::OnStarted()
{
    std::packaged_task<bool()> task(std::bind(&ClientWorker::DoTask, this));

    m_workResult = task.get_future();
    m_workerThread = std::thread(std::move(task));

    m_analyzer->SetCapture(this,
        [this](std::unique_ptr<NetPacket> &&p)
    { this->InterceptPacket(std::forward<std::remove_reference<decltype(p)>::type>(p)); });

    return true;
}

void ClientWorker::HaltWorkThread()
{
    m_terminated = true;
    m_condvar.notify_one();

    if (m_workerThread.joinable())
        m_workerThread.join();
}

void ClientWorker::OnStopped()
{
    HaltWorkThread();
}



