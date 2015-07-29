/*
 * ClientMain.cpp -- client program
 */

#include "Client.hpp"

#include <iostream>
using namespace std;

#define PORT "3490"
#define SERVER_NAME "localhost"
#define MAXDATASIZE 512

int main(int argc, char** argv)
{
	char message[MAXDATASIZE];

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	client.ReceiveMessage(message, MAXDATASIZE);
	cout << "Following message received: " << message << endl;
	client.CloseConnection();
	cout << "Connection closed" << endl;
	return 0;
}
