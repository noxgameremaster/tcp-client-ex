
#include "serverReceiveEx.h"
#include "socketset.h"
#include "clientpool.h"
#include "winsocket.h"
#include "simpleBuffer.h"
#include "loopThread.h"

ServerReceiveEx::ServerReceiveEx()
    : NetService()
{
    m_socketSet = std::make_unique<SocketSet>();
    m_socketSet->SetTimeInterval(1, 0);
    m_recvBuffer.reserve(reserve_recv_buffer);
    m_receiveThread = std::make_unique<LoopThread>();
    m_receiveThread->SetTaskFunction([this]() { this->ReceiveData(); });
}

ServerReceiveEx::~ServerReceiveEx()
{ }

void ServerReceiveEx::OnDisconnected(WinSocket *client)
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_socketSet->Remove(client);
    }

    m_cliPool->Erase(client);
}

void ServerReceiveEx::ReceiveFromClient(WinSocket *client)
{
    m_recvBuffer.resize(reserve_recv_buffer);

    if (!client->Receive(m_recvBuffer))
        OnDisconnected(client);
    else if (m_simpBuffer)
        m_simpBuffer->PushBack(client, m_recvBuffer);
}

void ServerReceiveEx::ReceiveData()
{
    m_socketSet->DoSelect([this](WinSocket *s) { this->ReceiveFromClient(s); });
}

bool ServerReceiveEx::OnInitialize()
{
    if (!m_cliPool)
        return false;

    m_simpBuffer = std::make_shared<SimpleBuffer>();
    m_OnShareBuffer.Emit(m_simpBuffer);

    return true;
}

bool ServerReceiveEx::OnStarted()
{
    return m_receiveThread->Startup();
}

void ServerReceiveEx::OnDeinitialize()
{ }

void ServerReceiveEx::OnStopped()
{
    m_receiveThread->Shutdown();
}

void ServerReceiveEx::SlotNewUser(std::weak_ptr<WinSocket> client)
{
    if (client.expired())
        return;

    auto clientInstance = client.lock();

    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_socketSet->Append(clientInstance.get());
    }
}

void ServerReceiveEx::RegistClientPool(std::shared_ptr<ClientPool> &cliPool)
{
    if (!cliPool)
        return;

    m_cliPool = cliPool;
    m_cliPool->OnRegistUser().Connection(&ServerReceiveEx::SlotNewUser, this);
}

