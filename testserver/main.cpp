// tcp_server_client.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#define TCP_SERVER_CODE

#ifdef TCP_SERVER_CODE

#include <iostream>
#include <string>
#include "TCPServer.h"

using namespace std;

string serverIP = "127.0.0.1";

int main()
{
	TCPServer server(serverIP, 18590);

    if (server.RunTestServer())
        std::getchar();

    server.StopTestServer();

	return 0;
}

#else

#include "eventworker.h"
#include <iostream>
#include "TCPClient.h"
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {

    EventWorker::Instance().Start();
	TCPClient *client = new TCPClient;
	string msg = "a";
	string usernameEntered;


	cout << "Enter your username." << endl;
	cin >> usernameEntered;
	client->username = usernameEntered;

	if (client->initWinsock()) {

		client->connectSock();

		client->recvThread = thread([&] {
			client->threadRecv();
		});

		while (true) {
			getline(cin, msg);
			std::string messageToSend;
			if (client->joinChat == false) {
				std::ostringstream ss;
				ss << client->username << ": " << msg;
				messageToSend = ss.str();
			}
			else if (client->joinChat == true) {
				std::ostringstream ss;
				ss << client->username << " joined the chat!";
				messageToSend = ss.str();
				client->joinChat = false;
			}
			client->sendMsg(messageToSend);
		}

	}


	delete client;
    EventWorker::Instance().Stop();
	cin.get();
	return 0;

}

#endif