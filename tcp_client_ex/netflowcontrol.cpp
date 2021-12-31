
#include "netflowcontrol.h"
#include "taskmanager.h"
#include "iobuffer.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "filepacketupload.h"
#include "loopThread.h"

NetFlowControl::NetFlowControl()
    : NetService()
{
    m_taskmanager = std::make_shared<TaskManager>(this);
    m_ioThread = std::make_unique<LoopThread>();

    m_ioThread->SetWaitCondition([this]() { return this->CheckHasIO(); });
    m_ioThread->SetTaskFunction([this]() { this->CheckIOList(); });
}

NetFlowControl::~NetFlowControl()
{ }

bool NetFlowControl::CheckHasIO() const
{
    std::lock_guard<std::mutex> guard(m_lock);

    return (m_inpacketList.size() + m_outpacketList.size()) > 0;
}

void NetFlowControl::CheckIOList()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    {
        std::lock_guard<std::mutex> lock(m_lock);
        while (m_inpacketList.size())
        {
            std::unique_ptr<NetPacket> packet = std::move(m_inpacketList.front());

            ReceivePacket(std::move(packet));
            m_inpacketList.pop_front();
        }
        while (m_outpacketList.size())
        {
            std::unique_ptr<NetPacket> packet = std::move(m_outpacketList.front());

            ReleasePacket(std::move(packet));
            m_outpacketList.pop_front();
        }
    }
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

void NetFlowControl::ReceivePacket(std::unique_ptr<NetPacket> &&packet)
{
    m_taskmanager->InputTask(std::move(packet));
}

bool NetFlowControl::ReleasePacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_sendbuffer.expired())
        return false;

    auto sendbuffer = m_sendbuffer.lock();

    std::unique_ptr<NetPacket> sendData = std::forward<std::remove_reference<decltype(packet)>::type>(packet);
    uint8_t *stream = nullptr;
    size_t length = 0;

    if (!sendData->Write(stream, length))
        return false;

    return sendbuffer->PushBuffer(stream, length);
}

void NetFlowControl::Enqueue(std::unique_ptr<NetPacket>&& packet, IOType ioType)
{
    std::list<std::remove_reference<decltype(packet)>::type> *ioList = nullptr;

    do
    {
        if (ioType == IOType::IN)
            ioList = &m_inpacketList;
        else if (ioType == IOType::OUT)
            ioList = &m_outpacketList;
        else
            break;

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
    std::unique_ptr<FilePacketUpload> filepack(new FilePacketUpload);

    filepack->SetUploadPath(fileInfo);
    filepack->ChangeSubCommand(FilePacketUpload::PacketSubCmd::TestSendToServer);
    Enqueue(std::move(filepack), IOType::IN);
}
