
#include "TCPServer.h"
#include "makepacket.h"
#include "serverfile.h"
#include "localbuffer.h"
#include "stringhelper.h"
#include "eventworker.h"
#include "runClock.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>

#define VISUAL_STUDIO_2015 1900
#if _MSC_VER == VISUAL_STUDIO_2015
#define NAMESPACE_FILESYSTEM std::experimental::filesystem
#else
#define NAMESPACE_FILESYSTEM std::filesystem
#endif
#undef VISUAL_STUDIO_2015

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
    m_makepacket->OnReceiveFileMeta().Connection(&TCPServer::OnReceiveFileMetaPacket, this);
    m_makepacket->OnReceiveFileChunk().Connection(&TCPServer::OnReceiveFileChunkPacket, this);

    m_servbuffer = std::make_shared<LocalBuffer>();
    m_buffer.resize(0x1000);

    m_clock = std::make_unique<RunClock>();
}

bool TCPServer::ServerHasFile(const std::string &path, const std::string &filename)
{
    if (!NAMESPACE_FILESYSTEM::is_directory(path))
        return false;

    return NAMESPACE_FILESYSTEM::exists(stringFormat("%s\\%s", path, filename));
}

bool TCPServer::ServerRemoteParseFileCommand(const std::string &remoteFileCmd, std::string &path, std::string &name)
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

void TCPServer::OnServerExecuteCommand(int senderSocket, const std::string &cmd, const size_t &cmdOffset)
{
    size_t findpos = cmd.find("/echo", cmdOffset);

    if (findpos != std::string::npos)
    {
        SendAllClient(&MakePacket::MakeEcho, [](SOCKET) { return true; }, "push echo message");
        std::cout << "server_side::echo\n";
    }

    findpos = cmd.find("/sendfile", cmdOffset);
    if (findpos != std::string::npos)
    {
        std::string path, filename;

        if (!ServerRemoteParseFileCommand(cmd.substr(findpos), path, filename))
            return;
        
        if (!ServerHasFile(path, filename))
        {
            m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeChat, stringFormat("server has no file %s or directory %s\n", filename, path), 10);
            return;
        }
        std::unique_ptr<ServerFile> file(new ServerFile(stringFormat("%s\\%s", path, filename)));

        if (!file)
            return;
                
        size_t filesize = static_cast<size_t>(NAMESPACE_FILESYSTEM::file_size(file->FileName()));

        m_servFile = std::move(file);
        SendAllClient(&MakePacket::MakeFileMeta, [sender = static_cast<SOCKET>(senderSocket)](SOCKET sock){ return sock != sender; }, filename, path, filesize);
        m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeChat, "server sent file meta data", 6);
        m_servFileName = filename;
        m_servPath = path;
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

void TCPServer::OnReceiveFileMetaPacket(int senderSocket)
{
    std::cout << stringFormat("server::onreceiveFilemetapacket\n");

    ////!test sending a chunk of file!//
    if (!m_servFile)
        return;

    /*std::vector<uint8_t> readbuff(512, 0);
    m_servFile->Read(readbuff);

    if (m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeFileChunk, m_servFileName, readbuff))
        std::cout << "server sent a chunk!" << std::endl;*/
    OnReceiveFileChunkPacket(senderSocket, false, false, 0);
    m_clock->Reset();
}

void TCPServer::OnReceiveFileChunkPacket(int senderSocket, bool isError, bool completed, size_t workpos)
{
    if (!m_servFile)
        return;

    do
    {
        if (isError)
            std::cout << "filechunk::geterror\n";
        else if (completed)
        {
            std::cout << "completed!\n";
            m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeChat, stringFormat("downloading is done. %s", m_clock->Show(true)), 2);
        }
        else
            break;
        m_servFile.reset();
        return;
    }
    while (false);

    std::vector<uint8_t> readbuff(1024, 0);

    if (!m_servFile->Read(readbuff))
        std::cout << "error?\n";
    if (readbuff.empty())
    {
        std::cout << "empty buffer\n";
        return;
    }

    if (m_makepacket->NetSendPacket(senderSocket, &MakePacket::MakeFileChunk, m_servFileName, readbuff))
        std::cout << stringFormat("server sent %d bytes", readbuff.size()) << std::endl;
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

    //m_makepacket->NetSendPacket(client, &MakePacket::MakeEcho, "echo msg");
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
    int bytesRead = recv(client, reinterpret_cast<char *>(m_buffer.data()), m_buffer.size(), 0);

    if (bytesRead <= 0)
    {
        OutOfClient(client);
        return;
    }
    
    m_makepacket->ReadPacket(client, reinterpret_cast<const char *>(m_buffer.data()), bytesRead);
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