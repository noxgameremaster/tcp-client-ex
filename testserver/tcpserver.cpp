
#include "TCPServer.h"
#include "makepacket.h"
#include <iostream>
#include <string>
#include <sstream>

const int MAX_BUFFER_SIZE = 4096;			//Constant value for the buffer size = where we will store the data received.


TCPServer::TCPServer()
{
	listenerPort = 18590;
	ServerInitialize();
}

TCPServer::TCPServer(std::string ipAddress, int port)
	: listenerIPAddress(ipAddress), listenerPort(port)
{
	ServerInitialize();
}

TCPServer::~TCPServer() {
	cleanupWinsock();			//Cleanup Winsock when the server shuts down. 
}

void TCPServer::ServerInitialize()
{
	m_packetProduce = std::make_unique<MakePacket>();
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
SOCKET TCPServer::createSocket() {

	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET = IPv4. 

	if (listeningSocket != INVALID_SOCKET) {

		sockaddr_in hint;		//Structure used to bind IP address & port to specific socket. 
		hint.sin_family = AF_INET;		//Tell hint that we are IPv4 addresses. 
		hint.sin_port = htons(listenerPort);	//Tell hint what port we are using. 
		inet_pton(AF_INET, listenerIPAddress.c_str(), &hint.sin_addr); 	//Converts IP string to bytes & pass it to our hint. hint.sin_addr is the buffer. 

		int bindCheck = bind(listeningSocket, (sockaddr *)&hint, sizeof(hint));	//Bind listeningSocket to the hint structure. We're telling it what IP address family & port to use. 

		if (bindCheck != SOCKET_ERROR) {			//If bind OK:

			int listenCheck = listen(listeningSocket, SOMAXCONN);	//Tell the socket is for listening. 
			if (listenCheck == SOCKET_ERROR) {
				return -1;
			}
		}

		else {
			return -1;
		}

		return listeningSocket;

	}

	return INVALID_SOCKET;

}


//Function doing the main work of the server -> evaluates sockets & either accepts connections or receives data. 
void TCPServer::run() {

	char buf[MAX_BUFFER_SIZE];		//Create the buffer to receive the data from the clients. 
	SOCKET listeningSocket = createSocket();		//Create the listening socket for the server. 

	while (true) {

		if (listeningSocket == INVALID_SOCKET) {
			break;
		}

		fd_set master;				//File descriptor storing all the sockets.
		FD_ZERO(&master);			//Empty file file descriptor. 

		FD_SET(listeningSocket, &master);		//Add listening socket to file descriptor. 

		while (true) {

			fd_set copy = master;	//Create new file descriptor bc the file descriptor gets destroyed every time. 
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);				//Select() determines status of sockets & returns the sockets doing "work". 

			for (int i = 0; i < socketCount; i++) {				//Server can only accept connection & receive msg from client. 

				SOCKET sock = copy.fd_array[i];					//Loop through all the sockets in the file descriptor, identified as "active". 

				if (sock == listeningSocket) {				//Case 1: accept new connection.

					SOCKET client = accept(listeningSocket, nullptr, nullptr);		//Accept incoming connection & identify it as a new client. 
					FD_SET(client, &master);		//Add new connection to list of sockets.  
					std::string welcomeMsg = "Welcome to Amine's Chat...\n";			//Notify client that he entered the chat. 
					send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
					std::cout << "New user joined the chat." << std::endl;			//Log connection on server side. 
					m_packetProduce->NetSendAll(client, "Welcome to Amine's Chat...\n");
					m_packetProduce->NetSendAll(client, "This is a Chat packet");

				}
				else {										//Case 2: receive a msg.	

					ZeroMemory(buf, MAX_BUFFER_SIZE);		//Clear the buffer before receiving data. 
					int bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0);	//Receive data into buf & put it into bytesReceived. 

					if (bytesReceived <= 0) {	//No msg = drop client. 
						closesocket(sock);
						FD_CLR(sock, &master);	//Remove connection from file director.
					}
					else {						//Send msg to other clients & not listening socket. 

						for (int i = 0; i < master.fd_count; i++) {			//Loop through the sockets. 
							SOCKET outSock = master.fd_array[i];

							if (outSock != listeningSocket) {

								if (outSock == sock) {		//If the current socket is the one that sent the message:
									std::string msgSent = "Message delivered.";
									send(outSock, msgSent.c_str(), msgSent.size() + 1, 0);	//Notify the client that the msg was delivered. 	
									m_packetProduce->NetSendAll(outSock, msgSent);
								}
								else {						//If the current sock is not the sender -> it should receive the msg. 
									//std::ostringstream ss;
									//ss << "SOCKET " << sock << ": " << buf << "\n";
									//std::string strOut = ss.str();
									send(outSock, buf, bytesReceived, 0);		//Send the msg to the current socket. 
									m_packetProduce->NetSendAll(outSock, buf);
								}

							}
						}

						std::cout << std::string(buf, 0, bytesReceived) << std::endl;			//Log the message on the server side. 

					}

				}
			}
		}


	}

}


//Function to send the message to a specific client. 
void TCPServer::sendMsg(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}


void TCPServer::cleanupWinsock()
{
	WSACleanup();
}

