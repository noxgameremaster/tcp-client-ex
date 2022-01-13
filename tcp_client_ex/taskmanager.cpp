
#include "taskmanager.h"
#include "netflowcontrol.h"
#include "taskthread.h"
#include "taskReportError.h"
#include "taskchatmessage.h"
#include "taskecho.h"
#include "taskfilestream.h"
#include "taskLargeFile.h"
#include "iobuffer.h"
#include "echopacket.h"
#include "filechunkpacket.h"
#include "filepacketupload.h"
#include "reportErrorPacket.h"
#include "largeFileChunkPacket.h"
#include "largeFileCompletePacket.h"
#include "largeFileRequestPacket.h"
#include "largefile.h"

TaskManager::TaskManager(NetObject *parent)
    : NetService(parent)
{
    m_netFlow = nullptr;
    m_taskthread = std::make_unique<TaskThread>(this);
}

TaskManager::~TaskManager()
{
}

bool TaskManager::SetNetFlowIO()
{
    NetObject *parent = GetParent();

    if (parent == nullptr)
        return false;

    m_netFlow = dynamic_cast<NetFlowControl *>(parent);
    return (m_netFlow != nullptr);
}

bool TaskManager::OnInitialize()
{
    m_largefile = std::make_shared<LargeFile>();
    
    std::unique_ptr<TaskFileStream> filetask(new TaskFileStream(this));

    filetask->OnReportFileMetaInfo().Connection(&LargeFile::SlotSetFileParams, m_largefile.get());
    filetask->OnReportReceiveFileChunk().Connection(&LargeFile::SlotWriteChunk, m_largefile.get());
    m_largefile->OnReportSetParamResult().Connection(&TaskFileStream::Report, filetask.get());
    m_largefile->OnWriteChunk().Connection(&TaskFileStream::ReportWriteChunk, filetask.get());
    //테스크 등록
    if (!SetNetFlowIO())
        return false;

    InsertTask(std::make_unique<TaskChatMessage>(this));
    InsertTask(std::make_unique<TaskEcho>(this));
    
    std::shared_ptr<AbstractTask> sharedFileTask(filetask.release());

    InsertSharedTask(sharedFileTask->TaskName(), sharedFileTask);
    InsertSharedTask(FileChunkPacket::TaskName(), sharedFileTask);
    InsertSharedTask(FilePacketUpload::TaskName(), sharedFileTask);

    std::shared_ptr<AbstractTask> largeFileTask(new TaskLargeFile(this));

    InsertSharedTask(LargeFileChunkPacket::TaskName(), largeFileTask);
    InsertSharedTask(LargeFileCompletePacket::TaskName(), largeFileTask);
    InsertSharedTask(LargeFileRequestPacket::TaskName(), largeFileTask);

    InsertTask(std::make_unique<TaskReportError>(this));

    return true;
}

bool TaskManager::InsertTask(std::unique_ptr<AbstractTask> &&task)
{
    std::string taskname = task->TaskName();

    if (GetTask(taskname) != nullptr)
        return false;

    m_taskmap.emplace(taskname, std::forward<std::remove_reference<decltype(task)>::type>(task));
    return true;
}

bool TaskManager::InsertSharedTask(const std::string &keyName, std::shared_ptr<AbstractTask> &sharedTask)
{
    if (GetTask(keyName) != nullptr)
        return false;

    m_taskmap.emplace(keyName, sharedTask);
    return true;
}

void TaskManager::OnDeinitialize()
{ }

bool TaskManager::OnStarted()
{
    SendOnInitial();
    m_taskthread->RunThread();
    return true;
}

void TaskManager::OnStopped()
{
    m_taskthread->StopThread();
}

void TaskManager::SendOnInitial()
{
    /*std::unique_ptr<EchoPacket> packet(new EchoPacket);

    packet->SetEchoMessage("connect completed");
    m_taskthread->PushBack(std::move(packet));*/
}

void TaskManager::InputTask(std::unique_ptr<NetPacket> &&packet)
{
    if (m_taskthread)
        m_taskthread->PushBack(std::move(packet));
}

AbstractTask *TaskManager::GetTask(const std::string &taskName)
{
    auto taskIterator = m_taskmap.find(taskName);

    if (taskIterator == m_taskmap.cend())
        return nullptr;

    return taskIterator->second.get();
}

void TaskManager::ForwardPacket(std::unique_ptr<NetPacket>&& packet, bool toInner)
{
    if (GetParent()!=nullptr)
        m_netFlow->Enqueue(std::forward<std::remove_reference<decltype(packet)>::type>(packet), 
            toInner ? NetFlowControl::IOType::INNER : NetFlowControl::IOType::OUT);
}


