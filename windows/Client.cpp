/*
* Client.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>

#include "Client.h"

Client::Client() {}

int Client::ConnectToServer(const char* serverName, const char* portNumber)
{
	WSADATA wsaData;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(serverName, portNumber, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	_sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (_sockfd == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	iResult = connect(_sockfd, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(_sockfd);
		_sockfd = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (_sockfd == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(_sockfd, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(_sockfd);
		WSACleanup();
		return 1;
	}

	return 0;
}

int Client::ReceiveMessage(char* message, int length)
{
	int numBytes;

	numBytes = recv(_sockfd, message, length, 0);
	if (numBytes < 0)
		printf("recv failed: %d\n", WSAGetLastError());

	return numBytes;
}

int Client::ReceiveMatrix(char* matrix, int rowCount, int colCount)
{
	int totalReceived = 0;
		
	while (totalReceived < rowCount*colCount)
	{
		totalReceived += ReceiveMessage(matrix + totalReceived, rowCount*colCount - totalReceived);
		if (totalReceived == 0)
			break;
	}

	return totalReceived;
}

void Client::CloseConnection()
{
	// cleanup
	closesocket(_sockfd);
	WSACleanup();
}