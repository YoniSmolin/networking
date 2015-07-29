/*
 * ClientMain.cpp -- client program
 */

#include "Client.hpp"

#include <iostream>
using namespace std;

#define PORT "3490"
#define SERVER_NAME "localhost"
#define MAXDATASIZE 512

#define MESSAGE "1111222233334444"
#define ROWS 4
#define COLS 4

int main(int argc, char** argv)
{
	string expectedMessage = MESSAGE;
	char message[MAXDATASIZE];

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	//client.ReceiveMessage(message, MAXDATASIZE);
	client.ReceiveMatrix(message, ROWS, COLS); 
	message[ROWS*COLS] = '\0';
	cout << "Following message received: " << message << endl;
	client.CloseConnection();
	if (expectedMessage.compare(message) == 0)
		cout << "This is the message we were expecting" << endl;
	else
		cout << "This is not the message we were expecting" << endl;
	cout << "Connection closed" << endl;
	return 0;
}
