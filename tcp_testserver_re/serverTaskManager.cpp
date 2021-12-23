
#include "serverTaskManager.h"
#include "serverTaskThread.h"
#include "serverEchoTask.h"
#include "serverChatTask.h"
#include "clientpool.h"
#include "chatPacket.h"
#include "winsocket.h"
#include "loopThread.h"
#include "printUtil.h"
#include "stringHelper.h"

using namespace _StringHelper;

ServerTaskManager::ServerTaskManager()
    : NetService()
{
    m_ioThread = std::make_unique<LoopThread>();
    m_ioThread->SetTaskFunction([this]() { this->DequeueIOList(); });
    m_servTaskThread = std::make_unique<ServerTaskThread>(this);
}

ServerTaskManager::~ServerTaskManager()
{ }

void ServerTaskManager::DequeueIOList()
{
    std::unique_ptr<NetPacket> packet;
    {
        std::lock_guard<std::mutex> guard(m_lock);

        while (m_inpacketList.size())
        {
            packet = std::move(m_inpacketList.front());
            m_servTaskThread->PushBack(std::move(packet));
            m_inpacketList.pop_front();
        }

        while (m_outpacketList.size())
        {
            packet = std::move(m_outpacketList.front());
            m_OnReleasePacket.Emit(std::move(packet));
            m_outpacketList.pop_front();
        }
    }
    std::this_thread::sleep_for(std::chrono::microseconds(30));
}

bool ServerTaskManager::InsertServerTask(std::unique_ptr<ServerTask> &&servTask)
{
    std::string taskname = servTask->TaskName();

    if (GetTask(taskname) != nullptr)
        return false;

    m_taskmap.emplace(taskname, std::move(servTask));
    return true;
}

bool ServerTaskManager::OnInitialize()
{
    if (!InsertServerTask(std::make_unique<ServerEchoTask>(this)))
        return false;
    if (!InsertServerTask(std::make_unique<ServerChatTask>(this)))
        return false;

    return true;
}

bool ServerTaskManager::OnStarted()
{
    m_servTaskThread->RunThread();
    return m_ioThread->Startup();
}

void ServerTaskManager::OnDeinitialize()
{ }

void ServerTaskManager::OnStopped()
{
    m_servTaskThread->StopThread();
    m_ioThread->Shutdown();
}

void ServerTaskManager::Enqueue(std::unique_ptr<NetPacket> &&packet, TaskIOType ioType)
{
    std::list<std::remove_reference<decltype(packet)>::type> *ioList = nullptr;

    do
    {
        if (ioType == TaskIOType::IN)
            ioList = &m_inpacketList;
        else if (ioType == TaskIOType::OUT)
            ioList = &m_outpacketList;
        else
            break;

        {
            std::lock_guard<std::mutex> lock(m_lock);

            ioList->push_back(std::move(packet));
        }
    }
    while (false);
}

ServerTask *ServerTaskManager::GetTask(const std::string &taskName)
{
    auto taskIterator = m_taskmap.find(taskName);

    if (taskIterator == m_taskmap.cend())
        return nullptr;

    return taskIterator->second.get();
}

void ServerTaskManager::ConnectWithClientPool(std::shared_ptr<ClientPool> &cliPool)
{
    if (!cliPool)
        return;

    cliPool->OnRegistUser().Connection(&ServerTaskManager::WhenNewConnection, this);
    cliPool->OnExitUser().Connection(&ServerTaskManager::WhenOccurredDisconnection, this);
}

void ServerTaskManager::WhenNewConnection(std::weak_ptr<WinSocket> client)
{
    if (client.expired())
        return;

    auto clientSock = client.lock();
    ChatPacket *msg = new ChatPacket;

    msg->SetChatMessage("Welcome a new test server! XD");
    msg->SetColorId(static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_PINK));
    msg->SetSenderSocketId(clientSock->GetFd());

    std::unique_ptr<NetPacket> pack(msg);

    Enqueue(std::move(pack), TaskIOType::OUT);

    ChatPacket *allMsg = new ChatPacket;

    allMsg->SetChatMessage(stringFormat("new user(%d) has joined in this server!", static_cast<int>(clientSock->GetFd())));
    allMsg->SetColorId(static_cast<uint8_t>(PrintUtil::ConsoleColor::COLOR_EMERALD));

    Enqueue(std::unique_ptr<NetPacket>(allMsg), TaskIOType::OUT);
}

void ServerTaskManager::WhenOccurredDisconnection(socket_type socketId)
{
    ChatPacket *allmsg = new ChatPacket;

    allmsg->SetChatMessage(stringFormat("user #%d has left from this server!", static_cast<int>(socketId)));
    allmsg->SetColorId(5);

    Enqueue(std::unique_ptr<NetPacket>(allmsg), TaskIOType::OUT);
}

