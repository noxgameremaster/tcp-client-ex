
#ifndef LOCAL_BUFFER_H__
#define LOCAL_BUFFER_H__

#include "binarystream.h"

class LocalBuffer : public BinaryStream
{
    static constexpr size_t local_buffer_reserve = 8192;
private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_seekpoint;

public:
    explicit LocalBuffer();
    ~LocalBuffer();

    bool IsEmpty() const;
    bool CheckRemaining(size_t length);
    bool Append(const uint8_t *src, size_t length);
    void Clear();
    void Pull(const size_t &count);

    template <class Container>
    bool Pop(Container &dest)
    {
        if (!m_seekpoint)
            return false;

        if (dest.size() < m_seekpoint)
            dest.resize(m_seekpoint);

        std::copy_n(m_buffer.begin(), m_seekpoint, dest.begin());
        return true;
    }

    template <class Container>
    bool PopN(Container &dest, uint32_t offset)
    {
        if (!m_seekpoint || dest.empty())
            return false;

        if (offset + dest.size() > m_seekpoint)
            return false;

        std::copy_n(&m_buffer[offset++], dest.size(), dest.begin());
        return true;
    }

    template <class T>
    bool PeekInc(T &dest, uint32_t &readpos)
    {
        if (readpos + sizeof(T) > m_seekpoint)
            return false;

        dest ^= dest;
        if (!GetStreamChunk(dest, readpos))
            return false;
        readpos += sizeof(T);
        return true;
    }

    template <class T>
    bool Peek(T &dest, uint32_t readpos)
    {
        if (readpos + sizeof(T) > m_seekpoint)
            return false;

        dest ^= dest;
        return GetStreamChunk(dest, readpos);
    }

};

#endif

