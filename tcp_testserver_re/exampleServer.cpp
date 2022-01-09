
#include "exampleServer.h"
#include "serverAccept.h"
#include "serverReceiveEx.h"
#include "serverTaskManager.h"
#include "bufferPopper.h"
#include "serverSend.h"
#include "clientpool.h"
#include "winsocket.h"
#include "eventworker.h"
#include "netLogObject.h"

ExampleServer::ExampleServer()
    : NetService()
{ }

ExampleServer::~ExampleServer()
{ }

bool ExampleServer::MakeServerSocket()
{
    std::unique_ptr<WinSocket> sock(new WinSocket);

    if (!sock->CreateSocket())
        return false;

    if (!sock->Bind())
        return false;

    if (!sock->Listen())
        return false;

    m_listenSocket = std::move(sock);
    return true;
}

bool ExampleServer::OnInitialize()
{
    EventWorker::Instance().Start();

    if (!NetService::OnInitialize())
        return false;
    if (!MakeServerSocket())
        return false;

    NetLogObject::LogObject().OnReleaseLogMessage().Connection(&ExampleServer::SlotServerPrintLog, this);

    std::shared_ptr<ClientPool> cliPool(new ClientPool);

    m_accept = std::make_unique<ServerAccept>(m_listenSocket);
    m_receive = std::make_unique<ServerReceiveEx>(m_listenSocket);
    m_servWorker = std::make_unique<BufferPopper>();
    m_servSend = std::make_unique<ServerSend>();
    m_serverTaskManager = std::make_unique<ServerTaskManager>();

    m_accept->SetParams(cliPool, {});
    m_receive->RegistClientPool(cliPool);
    m_servSend->RegistClientPool(cliPool);
    m_serverTaskManager->ConnectWithClientPool(cliPool);

    m_receive->OnShareBuffer().Connection(&BufferPopper::SlotRegistBuffer, m_servWorker.get());
    m_servWorker->OnReleasePacket().Connection(&ExampleServer::SlotForwardPacket, this);
    m_serverTaskManager->OnReleasePacket().Connection(&ExampleServer::SlotSendPacket, this);

    m_receive->OnReceiveData().Connection(&BufferPopper::SlotBufferPushed, m_servWorker.get());

    return true;
}

bool ExampleServer::OnStarted()
{
    if (!m_accept->Startup())
        return false;

    if (!m_receive->Startup())
        return false;

    if (!m_servWorker->Startup())
        return false;

    if (!m_servSend->Startup())
        return false;

    return m_serverTaskManager->Startup();
}

void ExampleServer::OnDeinitialize()
{
    auto netServiceStop = [](std::unique_ptr<NetService> &&service)
    {
        if (service)
            service->Shutdown();
    };

    netServiceStop(std::move(m_accept));
    netServiceStop(std::move(m_receive));
    netServiceStop(std::move(m_servWorker));
    netServiceStop(std::move(m_serverTaskManager));
    netServiceStop(std::move(m_servSend));

    NetService::OnDeinitialize();
}

void ExampleServer::OnStopped()
{    
    EventWorker::Instance().Stop();
}

void ExampleServer::SlotForwardPacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_serverTaskManager)
        m_serverTaskManager->Enqueue(std::move(packet), ServerTaskManager::TaskIOType::IN);
}

void ExampleServer::SlotSendPacket(std::unique_ptr<NetPacket> &&packet)
{
    if (m_servSend)
        m_servSend->Commit(std::move(packet));
}

void ExampleServer::SlotServerPrintLog(const std::string &msg, uint32_t colr)
{
    PrintUtil::PrintMessage(PrintUtil::GetPrintColor(colr), msg);
}

