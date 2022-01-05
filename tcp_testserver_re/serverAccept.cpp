
#include "serverAccept.h"
#include "socketset.h"
#include "clientpool.h"
#include "winsocket.h"
#include "loopThread.h"
#include <WS2tcpip.h>

ServerAccept::ServerAccept(std::shared_ptr<WinSocket> &listenSocket)
    : NetService()
{
    m_socketset = std::make_unique<SocketSet>();
    m_socketset->SetTimeInterval(1, 0);
    m_acceptThread = std::make_unique<LoopThread>();
    m_acceptThread->SetTaskFunction([this]() { return this->AcceptFromClient(); });
    m_listenSocket = listenSocket;
}

ServerAccept::~ServerAccept()
{ }

void ServerAccept::AcceptClient(WinSocket *client)
{
    if (*client == *m_listenSocket)
    {
        std::unique_ptr<WinSocket> nClient(new WinSocket);

        m_listenSocket->Accept(std::move(*nClient)); //Todo
        if (m_cliPool)
            m_cliPool->Append(std::move(nClient));
    }
}

bool ServerAccept::AcceptFromClient()
{
    m_socketset->DoSelect([this](WinSocket *c) { this->AcceptClient(c); });

    return true;
}

bool ServerAccept::OnInitialize()
{
    if (!m_listenSocket)
        return false;

    m_socketset->Append(m_listenSocket.get());
    return true;
}

bool ServerAccept::OnStarted()
{
    return m_acceptThread->Startup();
}

void ServerAccept::OnDeinitialize()
{ }

void ServerAccept::OnStopped()
{
    m_acceptThread->Shutdown();
}

void ServerAccept::SetParams(std::shared_ptr<ClientPool> &cliPool, std::shared_ptr<WinSocket> listenSocket)
{
    if (cliPool)
        m_cliPool = cliPool;
    if (listenSocket)
        m_listenSocket = listenSocket;
}