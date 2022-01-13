
#include "netobject.h"

#pragma comment(lib, "ws2_32.lib")

class NetObject::NetObjectImpl
{
    friend NetObject;
private:
    NetObject *m_owner;

public:
    explicit NetObjectImpl(NetObject *owner)
    {
        m_owner = owner;
    }
};

NetObject::NetObject(NetObject *parent)
{
    m_impl = std::make_shared<NetObjectImpl>(this);
    m_portNumber = 18590;
    m_ipString = "127.0.0.1";

    if (parent != nullptr)
        GetImpl(parent, m_parent);
}

NetObject::~NetObject()
{ }

void NetObject::GetImpl(std::weak_ptr<NetObjectImpl> &destImpl)
{
    GetImpl(this, destImpl);
}

void NetObject::GetImpl(NetObject *other, std::weak_ptr<NetObjectImpl> &destImpl)
{
    if (other != nullptr)
        destImpl = other->m_impl;
}

NetObject *NetObject::GetOther(std::weak_ptr<NetObjectImpl> &srcImpl)
{
    if (srcImpl.expired())
        return nullptr;

    auto other = srcImpl.lock();

    return other->m_owner;
}

NetObject *NetObject::GetParent()
{
    if (m_parent.expired())
        return nullptr;

    return GetOther(m_parent);
}

void NetObject::ShareOption(NetObject *netUnit)
{
    if (netUnit == nullptr)
        return;

    netUnit->m_ipString = m_ipString;
    netUnit->m_portNumber = m_portNumber;
}

void NetObject::SetNetOption(const std::string &ipAddress, uint16_t portId)
{
    if (ipAddress.empty())
        return;

    m_ipString = ipAddress;
    if (portId)
        m_portNumber = portId;
}

