
#include "asynchronousFileTask.h"
#include "ioFileStream.h"
#include "eventThread.h"

#include <iterator>

AsynchonousFileTask::AsynchonousFileTask(IOFileStream *file)
    : CCObject(), m_targetfile(file)
{
    m_lock = std::make_shared<std::mutex>();
    m_fileThread = std::make_unique<EventThread>();

    m_fileThread->SetCondition([this]() { return this->IsContained(); });
    m_fileThread->SetExecution([this]() { return this->AsyncWrite(); });
    m_fileThread->SetLocker(m_lock);

    m_fileThread->Startup();
}

AsynchonousFileTask::~AsynchonousFileTask()
{ }

bool AsynchonousFileTask::AsyncWrite()
{
    {
        std::lock_guard<std::mutex> lock(*m_lock);

        m_targetfile->Write(m_streamBuffer);
        m_streamBuffer.clear();
    }
    return true;
}

void AsynchonousFileTask::PushStream(const std::vector<uint8_t> &src)
{
    {
        std::unique_lock<std::mutex> lock(*m_lock);

        std::copy(src.cbegin(), src.cend(), std::insert_iterator<decltype(m_streamBuffer)>(m_streamBuffer, m_streamBuffer.end()));
    }
    m_fileThread->Notify();
}



