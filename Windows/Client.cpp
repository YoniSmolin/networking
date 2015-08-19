/*
* Client.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>

#include "Client.h"

#define BYTES_PER_COMPRESSED_PIXEL 4
#define BYTES_IN_HEADER 3
#define BITS_IN_BYTE 8 
#define LAST_TWO_LSBS 0x03
#define FIRST_MSB  0x80

Client::Client(int rowCount, int colCount) : _compressedImageBuffer(NULL) {	}

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

	return totalReceived;
}

int Client::ReceiveMatrix(float* matrix, int rowCount, int colCount)
{
	if (sizeof(float) != 4)
		throw "Server::ReceiveMatrix - size of float on this machine must be 32 bits\n";

	return ReceiveMatrix((char*)matrix, rowCount, sizeof(float)*colCount);
}

int Client::ReceiveCompressed(const uchar* reference, uchar* received, int rowCount, int colCount)
{
	if (_compressedImageBuffer == NULL)
		_compressedImageBuffer = new char[rowCount * colCount * BYTES_PER_COMPRESSED_PIXEL];

	// initialize received
	memcpy(received, reference, colCount * rowCount);

	uchar header[BYTES_IN_HEADER];
	int totalReceived = waitUntilReceived((char*)header, BYTES_IN_HEADER);
	if (totalReceived < BYTES_IN_HEADER) // totalReceived will be less than BYTES_IN_HEASER only if server has closed connection
		return 0;

	// reconstruct the length of the compressed image from the header
	int compressedLength = header[0];
	compressedLength +=    header[1] << BITS_IN_BYTE;
	compressedLength += header[2] << 2 * BITS_IN_BYTE;

	// reconstruct the image itself
	totalReceived = waitUntilReceived(_compressedImageBuffer, compressedLength);
	if (totalReceived < compressedLength) // totalReceived will be less than BYTES_IN_HEASER only if server has closed connection
		return 0;

	int indexCompressed = 0;
	int shift = 0;
	while (indexCompressed < compressedLength)
	{
		uchar firstByte = (uchar)_compressedImageBuffer[indexCompressed++];
		uchar secondByte = (uchar)_compressedImageBuffer[indexCompressed++];
		uchar thirdByte = (uchar)_compressedImageBuffer[indexCompressed++];
		char fourthByte = _compressedImageBuffer[indexCompressed++]; // fourth byte is signed

		int imageIndex = firstByte + (secondByte << BITS_IN_BYTE) + ((thirdByte & LAST_TWO_LSBS) << BITS_IN_BYTE *2); 
		received[imageIndex] += (((int)fourthByte) << 1) + ((thirdByte & FIRST_MSB) >> (BITS_IN_BYTE - 1)); // the casts in this line are very important - think them through when reading
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
	if (_compressedImageBuffer != NULL)
		delete[] _compressedImageBuffer;
}