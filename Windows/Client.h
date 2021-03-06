/*
** Client.h - a header file for the server class
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>

typedef unsigned char uchar;

class Client
{
	SOCKET _sockfd = INVALID_SOCKET;

public:

	int ConnectToServer(const char* serverName, const char* portNumber);
	void CloseConnection();

	~Client();

protected:
	int ReceiveMessage(char* message, int length);
	int waitUntilReceived(char* buffer, int length); // will not return before length bytes are received
};

#endif
