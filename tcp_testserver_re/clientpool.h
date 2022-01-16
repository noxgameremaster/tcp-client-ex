
#ifndef CLIENT_POOL_H__
#define CLIENT_POOL_H__

#include "netobject.h"
#include <map>
#include <mutex>

class WinSocket;

class ClientPool : public NetObject
{
    using client_pool_type = std::map<socket_type, std::shared_ptr<WinSocket>>;
    using client_pool_iterator = client_pool_type::iterator;
private:
    client_pool_type m_cliPool;

public:
    explicit ClientPool();
    ~ClientPool() override;

private:
    bool FindSocket(socket_type socketId, std::function<void(client_pool_iterator)> &&f = {});

public:
    bool Append(std::unique_ptr<WinSocket> &&client);
    bool Erase(WinSocket *sock);
    bool FindUserFromId(socket_type socketId, std::weak_ptr<WinSocket> &getsocket);
    std::list<socket_type> GetSocketIdList() const;

public:
    DECLARE_SIGNAL(OnRegistUser, std::weak_ptr<WinSocket>)
public:
    DECLARE_SIGNAL(OnExitUser, socket_type)

private:
    mutable std::mutex m_lock;
};

#endif

