#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__


#include <string>
#include <memory>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class TCPServer;
class MakePacket;

//Callback fct = fct with fct as parameter.
typedef void(*MessageReceivedHandler)(TCPServer *listener, int socketID, std::string msg);

class TCPServer {
private:
	std::unique_ptr<MakePacket> m_packetProduce;

public:
	TCPServer();
	TCPServer(std::string ipAddress, int port);
	~TCPServer();

private:
	void ServerInitialize();

public:
	void sendMsg(int clientSocket, std::string msg);
	bool initWinsock();
	void run();
	void cleanupWinsock();


private:
	SOCKET createSocket();
	std::string listenerIPAddress;
	int listenerPort;
	//MessageReceivedHandler messageReceived; 
};

#endif

