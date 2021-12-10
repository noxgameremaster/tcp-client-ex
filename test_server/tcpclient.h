#pragma once

#include "ccobject.h"

#include <string>
#include <thread>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class MakePacket;

typedef void(*MessageReceivedHandler)(std::string msg);

class TCPClient : public CCObject
{
public:

	TCPClient();
	~TCPClient() override;
	bool initWinsock();
	void connectSock();
	void sendMsg(std::string txt);
	std::thread recvThread;
	void threadRecv();
	std::string username;
	bool joinChat = true;
    std::unique_ptr<MakePacket> m_makepacket;

private:
    void OnReceiveUnknownPacket(int senderSocket, const char *unknownStream, const size_t &length);
    void OnReceiveChatPacket(int senderSocket, const std::string &msg);
    void OnReceiveEchoPacket(int senderSocket, const std::string &echo);
    void UnknownPacketType(int senderSocket, uint8_t packetId);

private:
	SOCKET createSocket();
	std::string serverIP = "127.0.0.1";
	int serverPort = 18590;
	sockaddr_in hint;
	SOCKET serverSocket;		//This is the socket we will connect to. 
	bool recvThreadRunning;

private:
    std::mutex m_lock;

};
