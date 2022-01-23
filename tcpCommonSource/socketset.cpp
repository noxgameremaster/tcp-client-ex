
#include "socketset.h"
#include "winsocket.h"
#include <ws2tcpip.h>
#include <map>

struct SocketSet::SocketSetPimpl
{
    std::map<socket_type, std::weak_ptr<NetObject::NetObjectImpl>> m_socketmap;
};

SocketSet::SocketSet()
    : NetObject()
{
    m_pimpl = std::make_unique<SocketSetPimpl>();
    m_fdset = std::make_unique<fd_set>();
    FD_ZERO(m_fdset.get());
}

SocketSet::~SocketSet()
{ }

bool SocketSet::FindSocket(socket_type socketId, WinSocket **findsocket)
{
    auto socketIterator = m_pimpl->m_socketmap.find(socketId);

    if (socketIterator == m_pimpl->m_socketmap.cend())
        return false;

    if (socketIterator->second.expired())
    {
        m_pimpl->m_socketmap.erase(socketIterator);
        FD_CLR(socketId, m_fdset.get());
        return false;
    }
    if (findsocket != nullptr)
        *findsocket = dynamic_cast<WinSocket *>(GetOther(socketIterator->second));

    return true;
}

bool SocketSet::Append(WinSocket *sock)
{
    if (sock == nullptr)
        return false;

    socket_type socketId = sock->GetFd();

    if (FindSocket(socketId))
        return false;

    std::weak_ptr<NetObjectImpl> otherPl;

    GetImpl(sock, otherPl);
    m_pimpl->m_socketmap.emplace(socketId, otherPl);
    FD_SET(socketId, m_fdset.get());

    return true;
}

bool SocketSet::Remove(WinSocket *sock)
{
    if (sock == nullptr)
        return false;

    auto socketIterator = m_pimpl->m_socketmap.find(sock->GetFd());

    if (socketIterator == m_pimpl->m_socketmap.cend())
        return false;

    m_pimpl->m_socketmap.erase(socketIterator);
    FD_CLR(sock->GetFd(), m_fdset.get());
    return true;
}

fd_set *SocketSet::Raw()
{
    m_copiedset = std::make_unique<fd_set>(*m_fdset);

    return m_copiedset.get();
}

void SocketSet::Clear()
{
    m_pimpl->m_socketmap.clear();
    FD_ZERO(m_fdset.get());
}

void SocketSet::SetTimeInterval(int seconds, int milliseconds)
{
    m_timeInterval = std::make_unique<timeval>();

    m_timeInterval->tv_sec = seconds;
    m_timeInterval->tv_usec = milliseconds;
}

void SocketSet::SetNullInterval()
{
    if (m_timeInterval)
        m_timeInterval.reset();
}

const timeval *SocketSet::Interval()
{
    return m_timeInterval.get();
}

std::list<WinSocket *> SocketSet::NotifiedClientList()
{
    if (!m_copiedset)
        return {};

    std::list<WinSocket *> clients;
    uint32_t sockcount = m_copiedset->fd_count;
    WinSocket *socket = nullptr;

    while (sockcount--)
    {
        if (!FindSocket(m_copiedset->fd_array[sockcount], &socket))
            continue;

        clients.push_back(socket);
    }
    return clients;
}

bool SocketSet::DoSelect(std::function<bool(WinSocket *)> &&f)
{
    if (m_pimpl->m_socketmap.empty())
        return false;

    int result = select(0, Raw(), nullptr, nullptr, m_timeInterval.get());

    //int error = WSAGetLastError();
    if (SOCKET_ERROR == result)
        return false;
    if (!result)
        return true;   //timeout

    auto clist = NotifiedClientList();

    for (auto client : clist)
    {
        if (!f(client))
            return false;
    }

    return true;
}

