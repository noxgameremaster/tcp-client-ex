
#include "taskLargeFile.h"
#include "largeFileRequestPacket.h"
#include "largeFileChunkPacket.h"
#include "largeFileCompletePacket.h"
#include "netLogObject.h"
#include "ioFileStream.h"

TaskLargeFile::TaskLargeFile(NetObject *parent)
    : AbstractTask(parent)
{ }

TaskLargeFile::~TaskLargeFile()
{ }

bool TaskLargeFile::CreateDownloadFile(const std::string &url)
{
    m_file = std::make_unique<IOFileStream>(url);

    return true;
}

void TaskLargeFile::RequestChunkData()
{
    std::unique_ptr<LargeFileChunkPacket> reqChunk(new LargeFileChunkPacket);

    reqChunk->ChangeSubCommand(LargeFileChunkPacket::PacketSubCmd::SendToServer);
    ForwardPacketToManager(std::move(reqChunk));
}

void TaskLargeFile::ReportDownloadComplete()
{
    std::unique_ptr<LargeFileCompletePacket> complete(new LargeFileCompletePacket);

    ForwardPacketToManager(std::move(complete));
}

void TaskLargeFile::LargeFileRequest(std::unique_ptr<NetPacket> &&reqPacket)
{
    LargeFileRequestPacket *req = dynamic_cast<LargeFileRequestPacket *>(reqPacket.get());

    switch (req->SubCommand())
    {
    case LargeFileRequestPacket::PacketSubCmd::StartTestToServer:
        NetLogObject::LogObject().AppendLogMessage("sent request packet to server!");
        req->ChangeSubCommand(LargeFileRequestPacket::PacketSubCmd::SendToServer);
        ForwardPacketToManager(std::move(reqPacket));
        CreateDownloadFile(req->RequestFileUrl());
        break;
    case LargeFileRequestPacket::PacketSubCmd::SendToClient:
        NetLogObject::LogObject().AppendLogMessage("receive msg from server. i will request a chunk data!");
        RequestChunkData();
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
        m_file->Write(stream);
        
        RequestChunkData();
        break;
    case LargeFileChunkPacket::PacketSubCmd::SentLastDataToClient:
        chunk->GetFileStream(stream);
        m_file->Write(stream);
        ReportDownloadComplete();
        NetLogObject::LogObject().AppendLogMessage("end all!");
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

