
#ifndef TASK_FILE_STREAM_H__
#define TASK_FILE_STREAM_H__

#include "abstracttask.h"

class LargeFile;

class TaskFileStream : public AbstractTask
{
private:
    std::string m_filename;
    std::string m_pathname;
    size_t m_filesize;
    std::weak_ptr<LargeFile> m_largefile;

    uint8_t m_error;

public:
    TaskFileStream(NetObject *parent);
    ~TaskFileStream();

private:
    void WriteFileMetaData();
    void GetLargeFileInstance();
    void ReportFileMetaReceiveCompleted();
    void Report(bool noError);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;

public:
    static std::string TaskName();
};

#endif

