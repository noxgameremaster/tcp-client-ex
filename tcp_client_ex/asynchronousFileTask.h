
#ifndef ASYNCHRONOUS_FILE_TASK_H__
#define ASYNCHRONOUS_FILE_TASK_H__

#include "ccobject.h"
#include <vector>

class EventThread;
class IOFileStream;

class AsynchonousFileTask : public CCObject
{
private:
    std::unique_ptr<EventThread> m_fileThread;
    std::vector<uint8_t> m_streamBuffer;
    std::unique_ptr<IOFileStream> m_targetfile;

public:
    explicit AsynchonousFileTask(IOFileStream *file);
    ~AsynchonousFileTask() override;

private:
    bool IsContained() const
    {
        return m_streamBuffer.size() > 0;
    }
    bool AsyncWrite();

public:
    void PushStream(const std::vector<uint8_t> &src);
    IOFileStream *operator->()
    {
        return m_targetfile.get();
    }

private:
    std::shared_ptr<std::mutex> m_lock;
};

#endif

