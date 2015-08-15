/*
** Client.h - a header file for the server class
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>

class Client
{
	SOCKET _sockfd = INVALID_SOCKET;

public:
	Client();

	int ConnectToServer(const char* serverName, const char* portNumber);
	int ReceiveMessage(char* message, int length);
	int ReceiveMatrix(char* matrix, int rowCount, int colCount);
	int ReceiveMatrix(float* matrix, int rowCount, int colCount);
	void CloseConnection();

private:
	void sigchld_handler(int s);
	void* get_in_addr(struct sockaddr *sa);

};

#endif