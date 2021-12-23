
#include "clientpool.h"
#include "winsocket.h"
#include "eventworker.h"

ClientPool::ClientPool()
    : NetObject()
{ }

ClientPool::~ClientPool()
{ }

bool ClientPool::FindSocket(socket_type socketId, std::function<void(client_pool_iterator)> &&f)
{
    {
        std::lock_guard<std::mutex> scope(m_lock);
        auto clientIterator = m_cliPool.find(socketId);

        if (clientIterator == m_cliPool.cend())
            return false;

        if (f)
            f(clientIterator);
    }
    return true;
}

bool ClientPool::Append(std::unique_ptr<WinSocket> &&client)
{
    socket_type sockId = client->GetFd();

    if (FindSocket(sockId))
        return false;

    std::shared_ptr<WinSocket> addClient = std::forward<std::remove_reference<decltype(client)>::type>(client);

    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_cliPool.emplace(sockId, addClient);
    }
    EventWorker::Instance().AppendTask(&m_OnRegistUser, addClient);
    return true;
}

bool ClientPool::Erase(WinSocket *sock)
{
    socket_type sockId = sock->GetFd();

    if (!FindSocket(sockId,
        [this](client_pool_iterator poolIterator)
    {
        m_cliPool.erase(poolIterator);
    }
        ))
        return false;

    EventWorker::Instance().AppendTask(&m_OnExitUser, sockId);
}

bool ClientPool::FindUserFromId(socket_type socketId, std::weak_ptr<WinSocket> &getsocket)
{
    if (!FindSocket(socketId, 
        [this, &getsocket](client_pool_iterator poolIterator)
    {
        getsocket = poolIterator->second;
    }
        ))
        return false;

    return !getsocket.expired();
}

std::list<socket_type> ClientPool::GetSocketIdList() const
{
    std::list<socket_type> idList;

    {
        std::lock_guard<std::mutex> scope(m_lock);

        for (auto &poolPair : m_cliPool)
        {
            auto client = poolPair.second;

            idList.push_back(client->GetFd());
        }
    }

    return idList;
}