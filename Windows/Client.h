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
	Client(int rowCound, int colCount);

	int ConnectToServer(const char* serverName, const char* portNumber);

	// it is assumed that the received message buffer is preallocated by the caller
	int ReceiveMessage(char* message, int length);
	int ReceiveMatrix(char* matrix, int rowCount, int colCount);
	int ReceiveMatrix(float* matrix, int rowCount, int colCount);
	int ReceiveCompressed(const uchar* reference, uchar* received, int rowCount, int colCount); 
	
	void CloseConnection();

	~Client();

private:
	int waitUntilReceived(char* buffer, int length); // will not return before length bytes are received
	
	void sigchld_handler(int s);
	void* get_in_addr(struct sockaddr *sa);

	char* _compressedImageBuffer;
};

#endif