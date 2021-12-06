
#ifndef WIN_SOCKET_H__
#define WIN_SOCKET_H__

#include "netobject.h"
#include <functional>
#include <string>

struct sockaddr_in;

class WinSocket : public NetObject
{
    using socket_deleter_type = std::function<void(socket_type *)>;
private:
    std::unique_ptr<socket_type, socket_deleter_type> m_socket;
    std::string m_ipAddr;
    uint16_t m_portId;
    std::unique_ptr<sockaddr_in> m_netHint;

public:
    explicit WinSocket(socket_type sock = NetObject::socket_error_val);
    ~WinSocket() override;

private:
    template <int ByteCount, int CurByte>
    struct EndianSwapper
    {
        template <class T>
        static void Swap(T &dest, T numeric)
        {
            dest |= (((numeric >> (CurByte << 3)) & 0xff) << ((ByteCount - CurByte - 1) << 3));
            EndianSwapper<ByteCount, CurByte - 1>::Swap(dest, numeric);
        }
    };

    template <int ByteCount>
    struct EndianSwapper<ByteCount, 0>
    {
        template <class T>
        static void Swap(T &dest, T numeric)
        {
            dest |= ((numeric & 0xff) << ((ByteCount - 1) << 3));
        }
    };

public:
    template <class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
    T SwapEndian(T numeric)
    {
        T dest = 0;
        static constexpr int byte_count = sizeof(T);

        EndianSwapper<byte_count, byte_count - 1>::Swap(dest, numeric);
        return dest;
    }

    bool IsValid();
    bool CreateSocket();

private:
    bool MakeHint();
    bool ReceiveImpl(char *buffer, const size_t length, int &readbytes);
    bool SendImpl(const char *buffer, const size_t length, int &sendbytes);
    bool SendAll(const char *buffer, const size_t length);
    bool SendAll(const uint8_t *ubuffer, const size_t length);

public:
    bool SetSocketOpt(const std::string &ipAddress, uint16_t portNumber);
    socket_type GetFd();
    bool Connect();
    bool Bind();
    bool Listen();

    template <class Container>
    bool Receive(Container &dest)
    {
        int readbytes = 0;
        bool receiveResult = ReceiveImpl(dest.data(), dest.size(), readbytes);

        if (receiveResult)
            dest.resize(readbytes);

        return receiveResult;
    }

    template <class Container>
    bool Send(const Container &src)
    {
        return SendAll(src.data(), src.size());
    }

    template <class CharT, class = typename std::enable_if<std::_Is_character<CharT>::value>::type>
    bool Send(const CharT *stream, const size_t length)
    {
        return SendAll(stream, length);
    }
};

#endif

