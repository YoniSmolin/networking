/*
* Client.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>

#include "Client.h"

#define BYTES_PER_COMPRESSED_PIXEL 3
#define BYTES_IN_HEADER 3
#define BYTE 8

Client::Client(int rowCount, int colCount) 
{
	_compressedImageBuffer = new char[rowCount * colCount * BYTES_PER_COMPRESSED_PIXEL];
}

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
	int expectedBytes = rowCount * colCount;
	int totalReceived = waitUntilReceived(matrix, expectedBytes);

	if (totalReceived < expectedBytes)
		return 0;
}

int Client::ReceiveMatrix(float* matrix, int rowCount, int colCount)
{
	if (sizeof(float) != 4)
		throw "Server::ReceiveMatrix - size of float on this machine must be 32 bits\n";

	return ReceiveMatrix((char*)matrix, rowCount, sizeof(float)*colCount);
}

int Client::ReceiveCompressed(const uchar* reference, uchar* received, int rowCount, int colCount)
{
	// initialize received
	memcpy(received, reference, colCount * rowCount);

	uchar header[BYTES_IN_HEADER];
	int totalReceived = waitUntilReceived((char*)header, BYTES_IN_HEADER);
	if (totalReceived < BYTES_IN_HEADER)
		return 0;

	// reconstruct the length of the compressed image from the header
	int compressedLength = header[0];
	compressedLength +=    header[1] << BYTE;
	compressedLength +=    header[2] << 2*BYTE;

	// reconstruct the image itself
	totalReceived = waitUntilReceived(_compressedImageBuffer, compressedLength);
	if (totalReceived < compressedLength)
		return 0;

	int indexCompressed = 0;
	while (indexCompressed < compressedLength)
	{
		int imageIndex = (uchar)_compressedImageBuffer[indexCompressed++]; // obtain 8 LSBs of index
		imageIndex += (uchar)_compressedImageBuffer[indexCompressed++] << BYTE; // obtain next 8 LSBs
		imageIndex += ((uchar)_compressedImageBuffer[indexCompressed] & 3) << BYTE * 2; // obtain final 2 most significant bits of the index are the lower 2 bits of this byte
		received[imageIndex] += _compressedImageBuffer[indexCompressed] >> 2; // the upper 6 bits are the difference which needs to be added to the reference image in order to decompress
		indexCompressed++;
	}

	return compressedLength + BYTES_IN_HEADER;
}

int Client::waitUntilReceived(char* buffer, int length)
{
	int totalReceived = 0;

	while (totalReceived < length)
	{
		int received = ReceiveMessage(buffer + totalReceived, length - totalReceived);
		if (received  == 0)
		{
			printf("Server closed connection");
			break;
		}
		totalReceived += received;
	}

	return totalReceived;
}

void Client::CloseConnection()
{
	// cleanup
	closesocket(_sockfd);
	WSACleanup();
}

Client::~Client()
{
	delete[] _compressedImageBuffer;
}