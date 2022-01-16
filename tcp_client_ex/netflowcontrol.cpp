
#include "netflowcontrol.h"
#include "taskmanager.h"
#include "iobuffer.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "chatPacket.h"
#include "largeFileRequestPacket.h"
#include "eventworker.h"
#include "netLogObject.h"
#include "loopThread.h"
#include "stringHelper.h"

using namespace _StringHelper;

NetFlowControl::NetFlowControl()
    : NetService()
{
    m_taskmanager = std::make_unique<TaskManager>(this);
    //m_ioThread = std::make_unique<LoopThread>(this);

    //m_ioThread->SetWaitCondition([this]() { return this->CheckHasIO(); });
    //m_ioThread->SetTaskFunction([this]() { return this->CheckIOList(); });

    m_ioCount = 0;

    m_debugCountIn = 0;
    m_debugCountOut = 0;
    m_debugCountEnqueueIn = 0;
    m_debugCountEnqueueOut = 0;

    m_terminated = false;
}

NetFlowControl::~NetFlowControl()
{ }

void NetFlowControl::IOThreadWork()
{
    do
    {
        {
            std::unique_lock<std::mutex> lock(m_lock);
            m_condvar.wait(lock, [this]() { return this->m_terminated || this->m_ioCount > 0; });

            if (m_terminated)
                break;
        }
        CheckIOList();
    }
    while (true);
}

bool NetFlowControl::CheckHasIO() const
{
    //std::lock_guard<std::mutex> guard(m_lock);

    //return (m_inpacketList.size() + m_outpacketList.size()) > 0;
    return m_ioCount > 0;
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
        if (m_ioCount > 0)
            --m_ioCount;
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
    if (m_ioThread.joinable())
    {
        m_terminated = true;
        m_condvar.notify_all();
        m_ioThread.join();
    }
    m_ioThread = std::thread([this]() { this->IOThreadWork(); });
    //return m_ioThread->Startup();
    return true;
}

void NetFlowControl::OnDeinitialize()
{ }

bool NetFlowControl::OnStarted()
{
    return m_taskmanager->Startup();
}

void NetFlowControl::OnStopped()
{
    //m_ioThread->Shutdown();
    m_terminated = true;
    m_condvar.notify_all();
    if (m_ioThread.joinable())
        m_ioThread.join();
    m_taskmanager->Shutdown();
}

void NetFlowControl::ReceivePacket(NetFlowControl::net_packet_type &&packet)
{
    m_taskmanager->InputTask(std::move(packet));
    ++m_debugCountIn;
}

bool NetFlowControl::ReleasePacket(NetFlowControl::net_packet_type &&packet)
{
    ++m_debugCountOut;
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
            ++m_debugCountEnqueueIn;
            break;
        case IOType::OUT:
            ioList = &m_outpacketList;
            ++m_debugCountEnqueueOut;
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
            ++m_ioCount;
        }
        //m_ioThread->Notify();
        m_condvar.notify_one();
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

void NetFlowControl::DebugReportInputOutputCounting()
{
    int ioCountCopy = m_ioCount;

    NET_PUSH_LOGMSG(stringFormat("in- %d, out- %d, enqueue in- %d, enqueue out- %d",
        m_debugCountIn, m_debugCountOut, m_debugCountEnqueueIn, m_debugCountEnqueueOut));
    NET_PUSH_LOGMSG(stringFormat("queueContain(in/out: %d/%d) %d %d", m_inpacketList.size(), m_outpacketList.size(), ioCountCopy, m_taskmanager->GetTaskCount()));
}