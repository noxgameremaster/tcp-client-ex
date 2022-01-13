
#ifndef WIN_SOCKET_H__
#define WIN_SOCKET_H__

#include "netobject.h"
#include <functional>
#include <string>
#include <type_traits>

struct sockaddr_in;

class WinSocket : public NetObject
{
    using socket_deleter_type = std::function<void(socket_type *)>;
public:
    static constexpr socket_type invalid_socket = static_cast<socket_type>(-1);

private:
    std::unique_ptr<socket_type, socket_deleter_type> m_socket;
    std::unique_ptr<sockaddr_in> m_netHint;
    uint32_t m_recvCount;
    uint32_t m_sendCount;

public:
    explicit WinSocket(socket_type sock = NetObject::socket_error_val);
    ~WinSocket() override;

    WinSocket &operator=(WinSocket &&movSock) noexcept;

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
    bool ReceiveImpl(uint8_t *buffer, const size_t length, int &readbytes);
    bool SendImpl(const char *buffer, const size_t length, int &sendbytes);
    bool SendAll(const char *buffer, const size_t length);
    bool SendAll(const uint8_t *ubuffer, const size_t length);

private:
    template <class Ty>
    struct is_char_type_impl
        : std::false_type
    {
    };

    template <>
    struct is_char_type_impl<char>
        : std::true_type
    {
    };

    template <>
    struct is_char_type_impl<unsigned char>
        : std::true_type
    {
    };

    template <class Ty>
    struct is_char_type
        : is_char_type_impl<typename std::remove_cv<Ty>::type>
    {
    };

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

    template <class CharT, class = typename std::enable_if<is_char_type<CharT>::value>::type>
    bool Send(const CharT *stream, const size_t length)
    {
        return SendAll(stream, length);
    }
    bool Accept(WinSocket &&dest);
    bool operator==(const WinSocket &other);

public:
    DECLARE_SIGNAL(OnReceive, uint32_t)
public:
    DECLARE_SIGNAL(OnSend, uint32_t)
};

#endif

