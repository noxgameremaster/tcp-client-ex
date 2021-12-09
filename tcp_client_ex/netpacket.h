
#ifndef NET_PACKET_H__
#define NET_PACKET_H__

#include "binarystream.h"

class HeaderData;

class NetPacket : public BinaryStream
{
protected:
    enum class Mode
    {
        None,
        Read,
        Write
    };

private:
    std::unique_ptr<HeaderData> m_headerData;

public:
    explicit NetPacket();
    ~NetPacket() override;

private:
    virtual bool OnReadPacket();
    virtual bool OnWritePacket();

protected:
    virtual size_t PacketSize(Mode mode);

public:
    void SetHeaderData(std::unique_ptr<HeaderData> &&headerData);
    bool Read();

private:
    bool WriteHeaderData(const size_t length);
    virtual uint8_t GetPacketId() const = 0;

public:
    bool Write();
    bool Write(uint8_t *&stream, size_t &length);

public:
    virtual void DoAction() {}
    virtual std::string ClassName() const = 0;

    template <class PacketObject>
    struct TaskKey
    {
        static std::string Get()
        {
            static_assert(std::is_base_of<NetPacket, PacketObject>::value, "the instance must be inherit NetPacket");

            return PacketObject::TaskName();
        }
    };
};

#endif

