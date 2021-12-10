
#include "TCPServer.h"
#include "makepacket.h"
#include "stringhelper.h"
#include "eventworker.h"
#include <iostream>
#include <sstream>

using namespace _StringHelper;

const int MAX_BUFFER_SIZE = 4096;			//Constant value for the buffer size = where we will store the data received.


TCPServer::TCPServer()
    : CCObject()
{
	m_listenerPort = 18590;
	ServerInitialize();
}

TCPServer::TCPServer(std::string ipAddress, int port)
	: m_listenerIPAddress(ipAddress), m_listenerPort(port), CCObject()
{
	ServerInitialize();
}

TCPServer::~TCPServer() {
	cleanupWinsock();			//Cleanup Winsock when the server shuts down. 
}

void TCPServer::ServerInitialize()
{
    m_halted = false;
    m_serverSet = std::make_unique<fd_set>();

    FD_ZERO(m_serverSet.get());
    m_interval = std::make_unique<timeval>();

    m_interval->tv_sec = 1;
    m_interval->tv_usec = 0;

    m_makepacket = std::make_unique<MakePacket>();

    m_makepacket->OnReceiveChat().Connection(&TCPServer::OnReceiveChatPacket, this);
    m_makepacket->OnReceiveEcho().Connection(&TCPServer::OnReceiveEchoPacket, this);
    m_makepacket->OnReceiveUnknown().Connection(&TCPServer::OnReceiveUnknownPacket, this);
    m_makepacket->OnUnknownPacketType().Connection(&TCPServer::UnknownPacketType, this);
    m_makepacket->OnSended().Connection(&TCPServer::NetSended, this);
}

void TCPServer::OnServerExecuteCommand(int senderSocket, const std::string &cmd, const size_t &cmdOffset)
{
    if (cmd.find("/echo", cmdOffset) != std::string::npos)
    {
        SendAllClient(&MakePacket::MakeEcho, [](SOCKET) { return true; }, "push echo message");
        std::cout << "server_side::echo\n";
    }
    if (cmd.find("/filemeta", cmdOffset) != std::string::npos)
    {
        SendAllClient(&MakePacket::MakeFileMeta, [sender = static_cast<SOCKET>(senderSocket)](SOCKET sock){ return sock != sender; }, "abc.txt", "echotest");
        m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeChat, "server sent file meta data", 6);
    }
}

void TCPServer::OnReceiveChatPacket(int senderSocket, const std::string &msg)
{
    std::string cmdKey = "/remote";
    size_t cmdEntry = msg.find(cmdKey);

    if (std::string::npos != cmdEntry)
    {
        OnServerExecuteCommand(senderSocket, msg, cmdEntry + cmdKey.length());
        return;
    }
    SendAllClient(&MakePacket::MakeChat, [](SOCKET) { return true; }, msg, 6);
    std::cout << stringFormat("message: %s\n", msg);
}

void TCPServer::OnReceiveEchoPacket(int senderSocket, const std::string &echo)
{
    std::cout << stringFormat("echo:: %s\n", echo);
}

void TCPServer::UnknownPacketType(int senderSocket, uint8_t packetId)
{
    std::cout << stringFormat("unknown type %d packet...\n", static_cast<int>(packetId));
}

void TCPServer::OnReceiveUnknownPacket(int senderSocket, std::unique_ptr<char[]> &&unknownStream, const size_t &length)
{
    std::string filterString = stringFormat("UNK::%s", MakePacket::filterPrint(unknownStream.get(), length));

    SendAllClient(&MakePacket::MakeChat, [](SOCKET) { return true; }, filterString, 6);
    std::cout << stringFormat("unknown packet stream: %s _ received", filterString) << std::endl;
}

void TCPServer::OnEnteredNewUser(SOCKET client)
{
    std::string msg = (client == INVALID_SOCKET) ? "server is down...\n" : stringFormat("entered a new user(%d)!", static_cast<int>(client));

    SendAllClient(&MakePacket::MakeChat, [client](SOCKET sock) { return sock != client; }, msg, 8);
}

//Function to check whether we were able to initialize Winsock & start the server. 
bool TCPServer::initWinsock() {

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);

	if (wsInit != 0) {
		std::cout << "Error: can't initialize Winsock." << std::endl;
		return false;
	}
	return true;
}

//Function that creates a listening socket of the server. 
bool TCPServer::createSocket() {

	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET = IPv4. 

	if (listeningSocket != INVALID_SOCKET) {

		sockaddr_in hint;		//Structure used to bind IP address & port to specific socket. 
		hint.sin_family = AF_INET;		//Tell hint that we are IPv4 addresses. 
		hint.sin_port = htons(m_listenerPort);	//Tell hint what port we are using. 
		inet_pton(AF_INET, m_listenerIPAddress.c_str(), &hint.sin_addr); 	//Converts IP string to bytes & pass it to our hint. hint.sin_addr is the buffer. 

		int bindCheck = bind(listeningSocket, (sockaddr *)&hint, sizeof(hint));	//Bind listeningSocket to the hint structure. We're telling it what IP address family & port to use. 

		if (bindCheck != SOCKET_ERROR) {			//If bind OK:

			int listenCheck = listen(listeningSocket, SOMAXCONN);	//Tell the socket is for listening. 
			if (listenCheck == SOCKET_ERROR) {
				return false;
			}
		}

		else {
			return false;
		}
        m_listenSocket = listeningSocket;
		return true;

	}

	return false;

}

void TCPServer::SelectTimeout()
{ }

bool TCPServer::SelectError()
{
    return false;
}

void TCPServer::EntryAccept()
{
    SOCKET client = accept(m_listenSocket, nullptr, nullptr);

    FD_SET(client, m_serverSet.get());

    m_makepacket->NetSendPacket(client, &MakePacket::MakeChat, "Welcome to Amine's Chat...\n", 6);
    m_makepacket->NetSendPacket(client, &MakePacket::MakeChat, "This is a Chat packet", 5);

    OnEnteredNewUser(client);
}

void TCPServer::OutOfClient(SOCKET client)
{
    FD_CLR(client, m_serverSet.get());
    closesocket(client);
}

void TCPServer::ReceiveFromClient(SOCKET client)
{
    m_buffer.fill(0);

    int bytesRead = recv(client, m_buffer.data(), m_buffer.size(), 0);

    if (bytesRead <= 0)
    {
        OutOfClient(client);
        return;
    }
    if (!m_makepacket->ReadPacket(static_cast<int>(client), m_buffer.data(), bytesRead))
    {
        std::cout << stringFormat("server::receive error. %d bytes received...\n", bytesRead);
    }
}

void TCPServer::ListenServer()
{
    fd_set copyset = *m_serverSet;
    int socketCount = select(0, &copyset, nullptr, nullptr, m_interval.get());

    if (socketCount == 0)
        SelectTimeout();
    else if (socketCount < 0)
        SelectError();
    else
    {
        int rep = -1;

        while ((++rep) < socketCount)
        {
            SOCKET sock = copyset.fd_array[rep];

            if (sock == m_listenSocket)
                EntryAccept();
            else
                ReceiveFromClient(sock);
        }
    }
}

void TCPServer::run()
{
    do
    {
        if (m_halted)
            break;

        ListenServer();
    }
    while (true);
}

//Function to send the message to a specific client. 
void TCPServer::sendMsg(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}


void TCPServer::cleanupWinsock()
{
    m_halted = true;

    if (m_serverWorker.joinable())
        m_serverWorker.join();

	WSACleanup();
}

template <class Function, class... Args>
void TCPServer::SendAllClient(Function &&f, std::function<bool(SOCKET)> &&cond, Args&&... args)
{
    int rep = m_serverSet->fd_count;

    while ((--rep) >= 0)
    {
        if (cond(m_serverSet->fd_array[rep]))
            m_makepacket->NetSendPacket(m_serverSet->fd_array[rep], std::forward<Function>(f), std::forward<Args>(args)...);
    }
}

void TCPServer::NetSended(const uint8_t *stream, const size_t length)
{
    std::cout << stringFormat("server send log:: %s\n", MakePacket::filterPrint(reinterpret_cast<const char *>(stream), length));
}

bool TCPServer::RunTestServer()
{
    if (!initWinsock())
        return false;
    if (!createSocket())		//Create the listening socket for the server. 
        return false;

    EventWorker::Instance().Start();

    FD_SET(m_listenSocket, m_serverSet.get());
    m_serverWorker = std::thread([this]() { this->run(); });

    return true;
}

void TCPServer::StopTestServer()
{
    if (m_listenSocket != INVALID_SOCKET)
        closesocket(m_listenSocket);

    cleanupWinsock();
    EventWorker::Instance().Stop();
}