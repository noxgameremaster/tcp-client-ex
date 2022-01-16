
#ifndef BINARY_BUFFER_H__
#define BINARY_BUFFER_H__

#include "binarystream.h"

class BinaryBuffer : public BinaryStream
{
private:
    std::vector<uint8_t> m_buffer;

public:
    explicit BinaryBuffer(const size_t &reserveSize);
    ~BinaryBuffer() override;

    size_t Size() const
    {
        return m_buffer.size();
    }

//private:
    bool AppendImpl(const uint8_t *streamPtr, const size_t length);

public:
    bool Append(const uint8_t src);

    template <class Ty>
    bool ReadChunk(Ty &dest)
    {
        try
        {
            ReadCtx(dest);
        }
        catch (const bool &fail)
        {
            return fail;
        }
        return true;
    }

    void Clear();
    
    template <class Ty = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool Back(Ty &dest)
    {
        if (m_buffer.empty())
            return false;

        uint32_t seekpoint = m_buffer.size() - sizeof(Ty);

        //auto seekpoint = GetSeekpoint();

        /*if (seekpoint < sizeof(Ty))
            return false;*/

        return GetStreamChunk(dest, /*seekpoint - sizeof(Ty)*/ seekpoint);
    }

    std::vector<uint8_t> GetPart(const size_t &requestLength, const size_t &off = 0);
    std::vector<uint8_t> GetPartRe(const size_t &requestLength, const size_t &off = 0);
};

#endif

