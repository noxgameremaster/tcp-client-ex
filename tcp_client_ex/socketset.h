
#ifndef SOCKET_SET_H__
#define SOCKET_SET_H__

#include "netobject.h"
#include <list>

struct fd_set;
struct timeval;
class WinSocket;

class SocketSet : public NetObject
{
    struct SocketSetPimpl;
private:
    std::unique_ptr<fd_set> m_fdset;
    std::unique_ptr<fd_set> m_copiedset;
    std::unique_ptr<SocketSetPimpl> m_pimpl;
    std::unique_ptr<timeval> m_timeInterval;

public:
    SocketSet();
    ~SocketSet() override;

private:
    bool FindSocket(socket_type socketId, WinSocket **findsocket = nullptr);

public:
    bool Append(WinSocket *sock);
    bool Remove(WinSocket *sock);
    fd_set *Raw();
    void Clear();
    void SetTimeInterval(int seconds, int milliseconds);
    void SetNullInterval();
    const timeval *Interval();
    std::list<WinSocket *> NotifiedClientList();
};

#endif

