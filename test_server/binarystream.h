
#ifndef BINARY_STREAM_H__
#define BINARY_STREAM_H__

#include <vector>
#include <stdexcept>

class BinaryStream
{
protected:
    using container_type = std::vector<uint8_t>;
    using container_pointer_type = container_type *;
    using value_type = container_type::value_type;

private:
    container_type m_binary;
    container_pointer_type m_context;
    uint32_t m_seekpoint;

public:
    explicit BinaryStream(const size_t reserve_size);
    virtual ~BinaryStream();

protected:
    void SetContext(container_pointer_type context)
    {
        m_context = context;
    }

private:
    template <size_t N>
    struct stream_chunk_modifier
    {
        static constexpr size_t lshift_count = (N - 1) * 8;

        template <class T>
        static void Get(const container_type &buffer, T &dest, int index)
        {
            stream_chunk_modifier<N - 1>::Get(buffer, dest, index - 1);

            dest |= (buffer.at(index) << lshift_count);
        }

        template <class T>
        static void Set(container_type &buffer, const T &src, int index)
        {
            stream_chunk_modifier<N - 1>::Set(buffer, src, index - 1);

            buffer.at(index) = (src >> lshift_count) & 0xff;
        }
    };

    template <>
    struct stream_chunk_modifier<1>
    {
        template <class T>
        static void Get(const container_type &buffer, T &dest, int index)
        {
            dest |= buffer.at(index);
        }

        template <class T>
        static void Set(container_type &buffer, const T &src, int index)
        {
            buffer.at(index) = src & 0xff;
        }
    };

protected:
    template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool GetStreamChunk(T &dest, int index)
    {
        static constexpr size_t type_size = sizeof(T);

        try
        {
            stream_chunk_modifier<type_size>::Get(*m_context, dest, index + type_size - 1);
        }
        catch (const std::out_of_range &/*oor*/)
        {
            return false;
        }

        return true;
    }

    template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool SetStreamChunk(const T &src, int index)
    {
        static constexpr size_t type_size = sizeof(T);

        try
        {
            stream_chunk_modifier<type_size>::Set(*m_context, src, index + type_size - 1);
        }
        catch (const std::out_of_range &)
        {
            return false;
        }
        return true;
    }

public:
    bool PutStreamRaw(const value_type *raw, size_t length);

protected:
    const value_type *GetRaw();
    void SetSeekpoint(uint32_t pos);
    uint32_t CurrentSeekpoint() const
    {
        return m_seekpoint;
    }
    void SetBufferSize(const size_t resize);
    size_t BufferSize() const
    {
        return m_context->size();
    }

    template <class T>
    void ReadCtx(T &dest)
    {
        if (!GetStreamChunk(dest, m_seekpoint))
            throw false;

        m_seekpoint += sizeof(T);
    }

    template <class T>
    void WriteCtx(const T &src)
    {
        if (!SetStreamChunk(src, m_seekpoint))
            throw false;

        m_seekpoint += sizeof(T);
    }

public:
    template <class Container>
    bool PutStream(const Container &cont)
    {
        if (m_context->capacity() < cont.size())
            return false;

        m_context->resize(cont.size());
        std::copy(cont.begin(), cont.end(), m_context->begin());
        return true;
    }
};

#endif

