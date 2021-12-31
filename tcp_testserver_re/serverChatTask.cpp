
#include "serverChatTask.h"
#include "chatPacket.h"
#include "echoPacket.h"
#include "filepacket.h"
#include "printUtil.h"
#include "ioFileStream.h"
#include "stringHelper.h"

using namespace _StringHelper;

ServerChatTask::ServerChatTask(NetObject *parent)
    : ServerTask(parent)
{ }

ServerChatTask::~ServerChatTask()
{ }

bool ServerChatTask::CheckValidColor(uint8_t colrbyte)
{
    return ((colrbyte > static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MIN)) && (colrbyte < static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_MAX)));
}

void ServerChatTask::SendPrivateMessage(const std::string &message, socket_type sendTo)
{
    std::unique_ptr<ChatPacket> chatmsg(new ChatPacket);

    chatmsg->SetChatMessage(message);
    chatmsg->SetColorId(static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_GREY));

    if (sendTo != send_all)
        chatmsg->SetSenderSocketId(sendTo);

    ForwardPacket(std::move(chatmsg));
}

bool ServerChatTask::ServerRemoteParseFileCommand(const std::string &remoteFileCmd, std::string &path, std::string &name)
{
    auto getCmdValue = [](const std::string &cmd, const std::string &findKey, std::string &dest)
    {
        size_t findpos = cmd.find(findKey);

        if (findpos == std::string::npos)
            return false;

        std::string findResult = cmd.substr(findpos + findKey.length());

        dest.reserve(findResult.size());
        for (const auto &c : findResult)
        {
            switch (c)
            {
            case ' ': case '\t': case '\n':
                break;
            default:
                dest.push_back(c);
            }
        }
        size_t slashpos = dest.find_first_of('/');

        if (slashpos != std::string::npos)
            dest = dest.substr(0, slashpos);
        return true;
    };

    if (!getCmdValue(remoteFileCmd, "/path", path))
        return false;
    if (!getCmdValue(remoteFileCmd, "/name", name))
        return false;

    return true;
}

void ServerChatTask::RemoteCommandEcho(socket_type sockId)
{
    std::unique_ptr<EchoPacket> echo(new EchoPacket);

    echo->SetEchoMessage("server response as echo...");
    echo->SetSenderSocketId(sockId);

    ForwardPacket(std::move(echo));
}

void ServerChatTask::RemoteCommandSendFile(const std::string &fileCmd, socket_type sockId)
{
    std::string path, fileName;

    if (!ServerRemoteParseFileCommand(fileCmd, path, fileName))
        return;

    std::string fullname = stringFormat("%s\\%s", path, fileName);
    size_t filesize = 0;

    if (!IOFileStream::FileSize(fullname, filesize))
    {
        SendPrivateMessage(stringFormat("server can't find a file '%s' or directory '%s'", fileName, path), sockId);
        return;
    }

    std::unique_ptr<FilePacket> sendfile(new FilePacket);

    sendfile->SetFileName(fileName);
    sendfile->SetSavePath(path);
    sendfile->SetFilesize(filesize);
    sendfile->SetFilePacketDirection(FilePacket::FilePacketDirection::ServerToClient);

    ForwardPacket(std::move(sendfile));

    m_OnServerRemoteFileInfo.Emit(path, fileName);
}

void ServerChatTask::RemoteExecuteCommand(const std::string &cmd, socket_type sockId)
{
    size_t findPos = cmd.find("/echo");

    if (findPos != std::string::npos)
        RemoteCommandEcho(sockId);

    const std::string fileKey = "/sendfile";

    findPos = cmd.find(fileKey);
    if (findPos != std::string::npos)
        RemoteCommandSendFile(cmd.substr(findPos + fileKey.length()), sockId);
}

void ServerChatTask::ConfirmMessage(const std::string &message, socket_type sockId)
{
    const std::string cmdKey = "/remote";
    size_t cmdEntry = message.find(cmdKey);

    if (cmdEntry != std::string::npos)
    {
        RemoteExecuteCommand(message.substr(cmdEntry + cmdKey.length()), sockId);
        return;
    }
    RemoteExecuteCommand(message, sockId);
    SendPrivateMessage(stringFormat("%d sent a message: %s", static_cast<int>(sockId), message));
}

void ServerChatTask::DoTask(std::unique_ptr<NetPacket> &&packet)
{
    ChatPacket *chat = dynamic_cast<ChatPacket *>(packet.get());

    if (chat == nullptr)
        return;

    std::string msg = chat->GetChatMessage();

    PrintUtil::PrintMessage(PrintUtil::ConsoleColor::COLOR_LIGHTGREEN, stringFormat("message: %s", msg));
    ConfirmMessage(msg, chat->SenderSocketId());
}

std::string ServerChatTask::TaskName() const
{
    return NetPacket::TaskKey<ChatPacket>::Get();
}

