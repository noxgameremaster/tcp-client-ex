
#include "netflowcontrol.h"
#include "taskmanager.h"
#include "iobuffer.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "chatPacket.h"
#include "largeFileRequestPacket.h"
#include "eventworker.h"
#include "loopThread.h"

NetFlowControl::NetFlowControl()
    : NetService()
{
    m_taskmanager = std::make_unique<TaskManager>(this);
    m_ioThread = std::make_unique<LoopThread>();

    m_ioThread->SetWaitCondition([this]() { return this->CheckHasIO(); });
    m_ioThread->SetTaskFunction([this]() { return this->CheckIOList(); });
}

NetFlowControl::~NetFlowControl()
{ }

bool NetFlowControl::CheckHasIO() const
{
    std::lock_guard<std::mutex> guard(m_lock);

    return (m_inpacketList.size() + m_outpacketList.size()) > 0;
}

void NetFlowControl::DequeueIO(NetFlowControl::net_packet_list_type &ioList, std::function<bool(NetFlowControl::net_packet_type&&)> &&processor)
{
    net_packet_type packet;
    std::unique_lock<std::mutex> localLock(m_lock);

    while (ioList.size())
    {
        packet = std::move(ioList.front());

        processor(std::move(packet));
        ioList.pop_front();
    }
}

bool NetFlowControl::CheckIOList()
{
    DequeueIO(m_inpacketList, [this](net_packet_type &&packet) { this->ReceivePacket(std::move(packet)); return true; });
    DequeueIO(m_outpacketList, [this](net_packet_type &&packet) { return this->ReleasePacket(std::move(packet)); });
    DequeueIO(m_innerPacketList, [this](net_packet_type &&packet) { this->ReleaseToInnerPacket(std::move(packet)); return true; });
    
    return true;
}

bool NetFlowControl::OnInitialize()
{
    return m_ioThread->Startup();
}

void NetFlowControl::OnDeinitialize()
{ }

bool NetFlowControl::OnStarted()
{
    return m_taskmanager->Startup();
}

void NetFlowControl::OnStopped()
{
    m_ioThread->Shutdown();
    m_taskmanager->Shutdown();
}

void NetFlowControl::ReceivePacket(NetFlowControl::net_packet_type &&packet)
{
    m_taskmanager->InputTask(std::move(packet));
}

bool NetFlowControl::ReleasePacket(NetFlowControl::net_packet_type &&packet)
{
    if (m_sendbuffer.expired())
        return false;

    auto sendbuffer = m_sendbuffer.lock();

    net_packet_type sendData = std::forward<std::remove_reference<decltype(packet)>::type>(packet);
    uint8_t *stream = nullptr;
    size_t length = 0;

    if (!sendData->Write(stream, length))
        return false;

    return sendbuffer->PushBuffer(stream, length);
}

void NetFlowControl::ReleaseToInnerPacket(NetFlowControl::net_packet_type &&innerPacket)
{
    QUEUE_EMIT(m_OnReleaseInnerPacket, net_shared_packet_type(std::move(innerPacket)));
}

void NetFlowControl::Enqueue(NetFlowControl::net_packet_type&& packet, IOType ioType)
{
    std::list<std::remove_reference<decltype(packet)>::type> *ioList = nullptr;

    do
    {
        switch (ioType)
        {
        case IOType::IN:
            ioList = &m_inpacketList;
            break;
        case IOType::OUT:
            ioList = &m_outpacketList;
            break;
        case IOType::INNER:
            ioList = &m_innerPacketList;
            break;
        default:
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_lock);

            ioList->push_back(std::move(packet));
        }
        m_ioThread->Notify();
    }
    while (false);
}

void NetFlowControl::SendEchoToServer(const std::string &echoMsg)
{
    std::unique_ptr<EchoPacket> echo(new EchoPacket);

    echo->SetEchoMessage(echoMsg);
    Enqueue(std::move(echo), IOType::IN);
}

void NetFlowControl::TestSendFilePacket(const std::string &fileInfo)
{
    std::unique_ptr<LargeFileRequestPacket> filepack(new LargeFileRequestPacket);

    filepack->SetRequestFileUrl(fileInfo);
    filepack->ChangeSubCommand(LargeFileRequestPacket::PacketSubCmd::StartTestToServer);
    Enqueue(std::move(filepack), IOType::IN);
}

void NetFlowControl::SendChatMessage(const std::string &msg)
{
    std::unique_ptr<ChatPacket> chat(new ChatPacket);

    chat->SetChatMessage(msg);
    Enqueue(std::move(chat), IOType::OUT);
}