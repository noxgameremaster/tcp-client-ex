
#ifndef SERVER_CHAT_TASK_H__
#define SERVER_CHAT_TASK_H__

#include "serverTask.h"

class IOFileStream;

class ServerChatTask : public ServerTask
{
    static constexpr socket_type send_all = static_cast<socket_type>(-1);
public:
    explicit ServerChatTask(NetObject *parent = nullptr);
    ~ServerChatTask() override;

private:
    bool CheckValidColor(uint8_t colrbyte);
    void SendPrivateMessage(const std::string &message, socket_type sendTo = send_all);
    bool ServerRemoteParseFileCommand(const std::string &remoteFileCmd, std::string &path, std::string &name);
    void RemoteCommandEcho(socket_type sockId);
    void RemoteCommandSendFile(const std::string &fileCmd, socket_type sockId);
    void RemoteCommandTripleMessage(const std::string &cmd, socket_type sockId);
    void RemoteExecuteCommand(const std::string &cmd, socket_type sockId);
    void ConfirmMessage(const std::string &message, socket_type sockId);
    void DoTask(std::unique_ptr<NetPacket> &&packet) override;

    std::string TaskName() const override;

    DECLARE_SIGNAL(OnServerRemoteFileInfo, std::string, std::string)
};

#endif

