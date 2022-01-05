
#ifndef SERVER_ACCEPT_H__
#define SERVER_ACCEPT_H__

#include "netservice.h"

class LoopThread;
class WinSocket;
class SocketSet;
class ClientPool;

class ServerAccept : public NetService
{
private:
    std::unique_ptr<LoopThread> m_acceptThread;
    std::shared_ptr<WinSocket> m_listenSocket;
    std::unique_ptr<SocketSet> m_socketset;
    std::shared_ptr<ClientPool> m_cliPool;

public:
    explicit ServerAccept(std::shared_ptr<WinSocket> &listenSocket);
    ~ServerAccept() override;

private:
    void AcceptClient(WinSocket *client);
    bool AcceptFromClient();
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;

public:
    void SetParams(std::shared_ptr<ClientPool> &cliPool, std::shared_ptr<WinSocket> listenSocket);
};

#endif

