
#ifndef NET_PACKET_H__
#define NET_PACKET_H__

#include "binarystream.h"

class HeaderData;

class NetPacket : public BinaryStream
{
private:
    std::unique_ptr<HeaderData> m_headerData;

public:
    explicit NetPacket();
    ~NetPacket() override;

private:
    virtual bool OnReadPacket();
    virtual bool OnWritePacket();

public:
    virtual size_t PacketSize();
    void SetHeaderData(std::unique_ptr<HeaderData> &&headerData);
    bool Read();

private:
    bool WriteHeaderData(const size_t length);
    virtual uint8_t GetPacketId() = 0;

public:
    bool Write();

public:
    virtual void DoAction() {}
    virtual std::string ClassName() const = 0;
};

#endif

