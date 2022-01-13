
#include "taskLargeFile.h"
#include "largeFileRequestPacket.h"
#include "largeFileChunkPacket.h"
#include "largeFileCompletePacket.h"
#include "netLogObject.h"
#include "ioFileStream.h"
#include "eventworker.h"
#include "stringHelper.h"

using namespace _StringHelper;

TaskLargeFile::TaskLargeFile(NetObject *parent)
    : AbstractTask(parent)
{
    m_accumulate = 0;
}

TaskLargeFile::~TaskLargeFile()
{ }

bool TaskLargeFile::CreateDownloadFile(const std::string &url)
{
    std::string filename, path;
    IOFileStream::UrlSeparatePathAndName(url, path, filename);

    m_file = std::make_unique<IOFileStream>(stringFormat("%s\\%s", "downloads", filename));

    return m_file->Open(IOFileStream::OpenMode::WriteOnly);
}

void TaskLargeFile::RequestChunkData()
{
    std::unique_ptr<LargeFileChunkPacket> reqChunk(new LargeFileChunkPacket);

    reqChunk->ChangeSubCommand(LargeFileChunkPacket::PacketSubCmd::SendToServer);
    ForwardPacketToManager(std::move(reqChunk));
}

void TaskLargeFile::ReportDownloadComplete()
{
    std::unique_ptr<IOFileStream> file(m_file.release());
    std::unique_ptr<LargeFileCompletePacket> complete(new LargeFileCompletePacket);

    complete->SetSubCmd();
    complete->SetLargeFileSize(static_cast<uint64_t>(m_accumulate), 0);
    ForwardPacketToManager(std::move(complete));
}

void TaskLargeFile::LargeFileRequest(std::unique_ptr<NetPacket> &&reqPacket)
{
    LargeFileRequestPacket *req = dynamic_cast<LargeFileRequestPacket *>(reqPacket.get());

    switch (req->SubCommand())
    {
    case LargeFileRequestPacket::PacketSubCmd::StartTestToServer:
        NET_PUSH_LOGMSG("sent request packet to server!");
        req->ChangeSubCommand(LargeFileRequestPacket::PacketSubCmd::SendToServer);
        ForwardPacketToManager(std::move(reqPacket));
        if (!CreateDownloadFile(req->RequestFileUrl()))
            NetLogObject::LogObject().AppendLogMessage("open fail!");
        break;
    case LargeFileRequestPacket::PacketSubCmd::SendToClient:
        NET_PUSH_LOGMSG("receive msg from server. i will request a chunk data!");
        RequestChunkData();
        m_accumulate = 0;
        break;
    }
}

void TaskLargeFile::LargeFileGetChunk(std::unique_ptr<NetPacket> &&pack)
{
    LargeFileChunkPacket *chunk = dynamic_cast<LargeFileChunkPacket *>(pack.get());
    std::vector<uint8_t> stream;

    switch (chunk->SubCommand())
    {
    case LargeFileChunkPacket::PacketSubCmd::SendToClient:
        chunk->GetFileStream(stream);
        if (!m_file->Write(stream))
            NET_PUSH_LOGMSG(stringFormat("error %d", stream.size()));
        
        RequestChunkData();
        m_accumulate += stream.size();
        break;
    case LargeFileChunkPacket::PacketSubCmd::SentLastDataToClient:
        chunk->GetFileStream(stream);
        if (!m_file->Write(stream))
            NET_PUSH_LOGMSG(stringFormat("error %d", stream.size()));
        m_accumulate += stream.size();
        ReportDownloadComplete();
        NET_PUSH_LOGMSG(stringFormat("end all %d bytes", m_accumulate));
        break;
    }
}

void TaskLargeFile::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    if (dynamic_cast<LargeFileRequestPacket *>(packet.get()) != nullptr)
        LargeFileRequest(std::move(packet));
    else if (dynamic_cast<LargeFileChunkPacket *>(packet.get()) != nullptr)
        LargeFileGetChunk(std::move(packet));
}

std::string TaskLargeFile::TaskName() const
{
    return NetPacket::TaskKey<LargeFileRequestPacket>::Get();
}

