#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "ccobject.h"

#include <string>
#include <thread>
#include <array>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class TCPServer;
class MakePacket;

//Callback fct = fct with fct as parameter.
typedef void(*MessageReceivedHandler)(TCPServer *listener, int socketID, std::string msg);

class TCPServer : public CCObject
{
    static constexpr size_t server_buffer_size = 4096;
private:
    std::string m_listenerIPAddress;
    int m_listenerPort;
    SOCKET m_listenSocket;
    std::thread m_serverWorker;
    bool m_halted;
    std::unique_ptr<fd_set> m_serverSet;
    std::unique_ptr<timeval> m_interval;
    std::array<char, server_buffer_size> m_buffer;
    std::unique_ptr<MakePacket> m_makepacket;

    std::string m_servFileName;
    std::string m_servPath;

public:
	TCPServer();
	TCPServer(std::string ipAddress, int port);
	~TCPServer();

private:
	void ServerInitialize();

private:
    bool ServerHasFile(const std::string &path, const std::string &filename);
    bool ServerRemoteParseFileCommand(const std::string &remoteFileCmd, std::string &path, std::string &name);
    void OnServerExecuteCommand(int senderSocket, const std::string &cmd, const size_t &cmdOffset);
    void OnReceiveChatPacket(int senderSocket, const std::string &msg);
    void OnReceiveEchoPacket(int senderSocket, const std::string &echo);
    void OnReceiveFileMetaPacket(int senderSocket);
    void UnknownPacketType(int senderSocket, uint8_t packetId);
    void OnReceiveUnknownPacket(int senderSocket, std::unique_ptr<char[]> &&unknownStream, const size_t &length);
    
    void OnEnteredNewUser(SOCKET client);

	void sendMsg(int clientSocket, std::string msg);
	bool initWinsock();
    void SelectTimeout();
    bool SelectError();
    void EntryAccept();
    void OutOfClient(SOCKET client);
    void ReceiveFromClient(SOCKET client);
    void ListenServer();
	void run();
	void cleanupWinsock();

    template <class Function, class... Args>
    void SendAllClient(Function &&f, std::function<bool(SOCKET)> &&cond, Args&&... args);

private:
    void NetSended(const uint8_t *stream, const size_t length);

public:
    bool RunTestServer();
    void StopTestServer();

private:
	bool createSocket();
	
	//MessageReceivedHandler messageReceived; 
};

#endif

