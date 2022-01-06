
#ifndef IO_BUFFER_H__
#define IO_BUFFER_H__

#include "partitionpool.h"
#include <list>
#include <vector>
#include <functional>
#include <mutex>

class IOBuffer : public PartitionPool
{
public:
    static constexpr size_t receive_buffer_max_size = 1024;

private:
    std::vector<uint8_t> m_largeBuffer;
    size_t m_largebufferRealsize;
    std::list<int> m_indexes;
    std::list<std::function<bool()>> m_triggers;

public:
    explicit IOBuffer();
    ~IOBuffer() override;

private:
    bool PopIndex(int &destIndex);
    bool StoreToLargebuffer(const int &srcIndex);
    void TriggeredWhenPush();

public:
    bool PushBuffer(const uint8_t *buffer, size_t bufferSize);

    template <class Container>
    bool PushBuffer(const Container &src)
    {
        return PushBuffer(src.data(), src.size());
    }

    bool PopBufferAlloc(std::unique_ptr<uint8_t[]> &&destptr, size_t &bufferSize);
    bool SetLargeBufferScale(size_t scale);
    bool IsEmpty() const;

    bool SetTrigger(NetObject *trigger, std::function<void()> &&fn);

private:
    mutable std::mutex m_lock;
    std::mutex m_trigLock;
};

#endif

