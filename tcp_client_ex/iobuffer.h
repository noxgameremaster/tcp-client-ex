
#ifndef IO_BUFFER_H__
#define IO_BUFFER_H__

#include "partitionpool.h"
#include <list>
#include <vector>
#include <functional>
#include <mutex>

class LocalBuffer;

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
    bool PushBuffer(uint8_t *buffer, size_t bufferSize);
    bool PopBuffer(const uint8_t *&destptr, size_t &bufferSize);
    bool SetLargeBufferScale(size_t scale);
    bool IsEmpty() const;

    bool SetTrigger(NetObject *trigger, std::function<void()> &&fn);

    void MoveBuffer(std::shared_ptr<LocalBuffer> localbuffer);

private:
    std::mutex m_lock;
};

#endif

