
#include "serverSend.h"
#include "loopThread.h"
#include "netpacket.h"
#include "clientpool.h"
#include "winsocket.h"

ServerSend::ServerSend()
    : NetService()
{
    m_sendThread = std::make_unique<LoopThread>();

    m_sendThread->SetTaskFunction([this]() { return this->SendLoop(); });
    m_sendThread->SetWaitCondition([this]() { return (!this->EmptySlot()); });
}

ServerSend::~ServerSend()
{ }

bool ServerSend::EmptySlot() const
{
    return m_packetList.empty();
}

bool ServerSend::SendPacket(std::unique_ptr<NetPacket> &&msg)
{
    if (m_cliPool.expired())
        return false;

    auto cliPool = m_cliPool.lock();
    uint8_t *stream = nullptr;
    size_t length = 0;

    if (!msg->Write(stream, length))
        return false;

    socket_type senderId = msg->SenderSocketId();
    std::weak_ptr<WinSocket> weakSock;

    if (senderId == static_cast<socket_type>(-1))
    {
        auto idList = cliPool->GetSocketIdList();

        for (const auto sockId : idList)
        {
            if (cliPool->FindUserFromId(sockId, weakSock))
            {
                if (!weakSock.lock()->Send(stream, length))
                    return false;
            }
        }
        return true;
    }

    if (!cliPool->FindUserFromId(senderId, weakSock))
        return false;

    auto destination = weakSock.lock();

    return destination->Send(stream, length);
}

bool ServerSend::SendLoop()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    {
        std::lock_guard<std::mutex> guard(m_lock);

        while (m_packetList.size())
        {
            SendPacket(std::move(m_packetList.front()));
            m_packetList.pop_front();
        }
    }
    return true;
}

bool ServerSend::OnInitialize()
{
    return true;
}

bool ServerSend::OnStarted()
{
    return m_sendThread->Startup();
}

void ServerSend::OnDeinitialize()
{ }

void ServerSend::OnStopped()
{
    m_sendThread->Shutdown();
}

void ServerSend::WhenJoinedNewConnect(std::weak_ptr<WinSocket> sock)
{ }

void ServerSend::Commit(std::unique_ptr<NetPacket> &&msg)
{
    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_packetList.push_back(std::move(msg));
    }
    m_sendThread->Notify();
}

void ServerSend::RegistClientPool(std::weak_ptr<ClientPool> cliPool)
{
    if (cliPool.expired())
        return;

    auto clients = cliPool.lock();

    clients->OnRegistUser().Connection(&ServerSend::WhenJoinedNewConnect, this);

    m_cliPool = cliPool;
}