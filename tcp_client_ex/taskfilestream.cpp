
#include "taskfilestream.h"
#include "taskmanager.h"
#include "filepacket.h"
#include "eventworker.h"

TaskFileStream::TaskFileStream(NetObject *parent)
    : AbstractTask(parent)
{
    m_error = 0;
    m_filesize = 0;
}

TaskFileStream::~TaskFileStream()
{ }

void TaskFileStream::WriteFileMetaData()
{
    EventWorker &work = EventWorker::Instance();

    work.AppendTask(&m_OnReportFileMetaInfo, m_filename, m_pathname, m_filesize);
}

void TaskFileStream::GetLargeFileInstance()
{
    /*if (m_largefile.expired())
    {
        TaskManager *taskman = dynamic_cast<TaskManager *>(GetParent());

        if (taskman == nullptr)
            return;

        taskman->GetLargeFileObject(m_largefile);
    }*/
}

void TaskFileStream::ReportFileMetaReceiveCompleted()
{
    std::unique_ptr<FilePacket> report(new FilePacket);

    report->SetError(m_error);
    report->SetFileName(m_filename);

    TaskManager *taskman = dynamic_cast<TaskManager *>(GetParent());

    if (taskman == nullptr)
        return;

    taskman->ForwardPacket(std::move(report));
}

void TaskFileStream::Report(bool noError)
{
    m_error = noError ? 0 : 1;
    //Todo. ���� ��Ÿ������ �޾����Ƿ� ������ �ϷẸ�� �ؾ��Ѵ�
    ReportFileMetaReceiveCompleted();
}

void TaskFileStream::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    FilePacket *metadata = dynamic_cast<FilePacket *>(packet.get());

    if (metadata == nullptr)
        return;

    //GetLargeFileInstance();

    m_filename = metadata->GetFileName();
    m_pathname = metadata->GetFilePath();
    m_filesize = metadata->GetFilesize();
    
    WriteFileMetaData();
}

std::string TaskFileStream::TaskName() const
{
    return NetPacket::TaskKey<FilePacket>::Get();
}
