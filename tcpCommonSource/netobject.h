
#ifndef NET_OBJECT_H__
#define NET_OBJECT_H__

#include "ccobject.h"

#include <string>

using socket_type = uint32_t;

class NetObject : public CCObject
{
protected:
    static constexpr socket_type socket_error_val = static_cast<socket_type>(-1);
    class NetObjectImpl;

private:
    std::string m_ipString;
    uint16_t m_portNumber;
    std::shared_ptr<NetObjectImpl> m_impl;
    std::weak_ptr<NetObjectImpl> m_parent;

public:
    explicit NetObject(NetObject *parent = nullptr);
    ~NetObject() override;

protected:
    void GetImpl(std::weak_ptr<NetObjectImpl> &destImpl);
    void GetImpl(NetObject *other, std::weak_ptr<NetObjectImpl> &destImpl);
    NetObject *GetOther(std::weak_ptr<NetObjectImpl> &srcImpl);
    NetObject *GetParent();
    void ShareOption(NetObject *netUnit);
    void SetNetOption(const std::string &ipAddress, uint16_t portId);

public:
    std::string GetIpAddress() const
    {
        return m_ipString;
    }
    uint16_t GetPortNumber() const
    {
        return m_portNumber;
    }
    virtual std::string ObjectName() const
    {
        return "NetObject";
    }
};

#endif

