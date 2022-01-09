
#ifndef SERVER_TASK_MANAGER_H__
#define SERVER_TASK_MANAGER_H__

#include "netservice.h"
#include <map>
#include <vector>

class NetPacket;
class LoopThread;
class ServerTaskThread;
class ServerTask;
class WinSocket;
class ClientPool;
class IOFileStream;

class ServerTaskManager : public NetService
{
private:
    std::list<std::unique_ptr<NetPacket>> m_inpacketList;
    std::list<std::unique_ptr<NetPacket>> m_outpacketList;
    std::unique_ptr<LoopThread> m_ioThread;
    std::unique_ptr<ServerTaskThread> m_servTaskThread;
    std::map<std::string, std::shared_ptr<ServerTask>> m_taskmap;

    std::unique_ptr<IOFileStream> m_servFile;
    
public:
    explicit ServerTaskManager();
    ~ServerTaskManager() override;

private:
    bool CheckHasIO() const;
    bool DequeueIOList();
    bool InsertServerTask(std::unique_ptr<ServerTask> &&servTask);
    bool InsertServerSharedTask(const std::string &taskKey, std::shared_ptr<ServerTask> sharedTask);
    bool OnInitialize() override;
    bool OnStarted() override;
    void OnDeinitialize() override;
    void OnStopped() override;
    void CreateServerFile(const std::string &path, const std::string &filename);
    void FetchFileStream(const std::string &filename);

public:
    enum class TaskIOType
    {
        IN,
        OUT
    };
    void Enqueue(std::unique_ptr<NetPacket> &&packet, TaskIOType ioType);
    ServerTask *GetTask(const std::string &taskName);

    void ConnectWithClientPool(std::shared_ptr<ClientPool> &cliPool);
    void WhenNewConnection(std::weak_ptr<WinSocket> client);
    void WhenOccurredDisconnection(socket_type socketId);

private:
    DECLARE_SIGNAL(OnReleasePacket, std::unique_ptr<NetPacket>&&)
    DECLARE_SIGNAL(OnReleaseFileStream, std::vector<uint8_t>, std::string)

private:
    mutable std::mutex m_lock;
};

#endif

