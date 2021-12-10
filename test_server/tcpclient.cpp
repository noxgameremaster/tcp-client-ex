#include "TCPClient.h"
#include "stringhelper.h"
#include "makepacket.h"
#include "eventworker.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace _StringHelper;


//When we create the client, we do not want the thread to run & try to receive data from the server until 
TCPClient::TCPClient()
    : CCObject()
{
	recvThreadRunning = false;
    m_makepacket = std::make_unique<MakePacket>();

    m_makepacket->OnReceiveChat().Connection(&TCPClient::OnReceiveChatPacket, this);
    m_makepacket->OnReceiveEcho().Connection(&TCPClient::OnReceiveEchoPacket, this);
}


TCPClient::~TCPClient()
{
	closesocket(serverSocket);
	WSACleanup();
	if (recvThreadRunning) {
		recvThreadRunning = false;
		recvThread.join();	//Destroy safely to thread. 
	}
}


bool TCPClient::initWinsock() {

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		cout << "Error: can't start Winsock." << endl;
		return false;
	}
	return true;
}

SOCKET TCPClient::createSocket() {

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cout << "Error: can't create socket." << endl;
		WSACleanup();
		return -1;
	}

	//Specify data for hint structure. 
	hint.sin_family = AF_INET;
	hint.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);

	return sock;

}

void TCPClient::threadRecv() {

	recvThreadRunning = true;
	while (recvThreadRunning) {

		char buf[1024];
		ZeroMemory(buf, sizeof(buf));

		int bytesReceived = recv(serverSocket, buf, sizeof(buf), 0);
		if (bytesReceived > 0) {			//If client disconnects, bytesReceived = 0; if error, bytesReceived = -1;

			//std::cout << string(buf, 0, bytesReceived) << std::endl;
            {
                std::lock_guard<std::mutex> guard(m_lock);

                if (!m_makepacket->ReadPacket(static_cast<int>(serverSocket), buf, bytesReceived))
                    std::cout << stringFormat("error! %d bytes received", bytesReceived) << std::endl;
            }

		}

	}
}

void TCPClient::OnReceiveUnknownPacket(int /*senderSocket*/, const char *unknownStream, const size_t &length)
{
    std::cout << stringFormat("unknown packet stream: %s _ received", MakePacket::filterPrint(unknownStream, length)) << std::endl;
}

void TCPClient::OnReceiveChatPacket(int /*senderSocket*/, const std::string &msg)
{
    std::cout << stringFormat("message: %s\n", msg);
}

void TCPClient::OnReceiveEchoPacket(int /*senderSocket*/, const std::string &echo)
{
    std::cout << stringFormat("echo: %s\n", echo);
}

void TCPClient::UnknownPacketType(int /*senderSocket*/, uint8_t packetId)
{
}

void TCPClient::connectSock() {

	//If !initWinsock -> return false. 

	serverSocket = createSocket();

	int connResult = connect(serverSocket, (sockaddr *)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cout << "Error: can't connect to server." << endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

}

void TCPClient::sendMsg(string txt) {

	if (!txt.empty() && serverSocket != INVALID_SOCKET) {

		//send(serverSocket, txt.c_str(), txt.size() + 1, 0);
        {
            std::lock_guard<std::mutex> guard(m_lock);

            m_makepacket->NetSendPacket(serverSocket, &MakePacket::MakeChat, txt, 6);
        }

		//It wouldn't work with the previous version bc while we were constantly listening for received msgs, we would keep caling this fct. 
		//This fct would send the message & try to handle the receiving too. It would get stuck while waiting for a received msg. 
	}

}