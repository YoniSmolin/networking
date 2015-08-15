#include <iostream>
#include <conio.h>
#include "Client.h"

#define SERVER_NAME "192.168.137.2"
#define PORT "3490"

#define MESSAGE_LENGTH 128

using namespace std;

int main()
{
	char message[MESSAGE_LENGTH];

	Client client;
	cout << "Initialized client successfully" << endl;

	if (client.ConnectToServer(SERVER_NAME, PORT) == 1)
		return 1;
	cout << "Successfully connected to server" << endl;

	int bytesReceived = client.ReceiveMessage(message, MESSAGE_LENGTH);

	if (bytesReceived < 0)
		return 1;

	if (bytesReceived > 0)
	{
		message[bytesReceived] = '\0';
		cout << "Received the following message: " << message << endl;
	}
	
	client.CloseConnection();
	cout << "Successfully closed client connection" << endl;

	cout << "Type any key to exit" << endl;

	while (!_kbhit());

	return 0;
}