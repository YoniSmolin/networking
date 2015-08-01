/*
** Client.cpp -- a TCP client class
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <iostream>

#include "Client.hpp"


using namespace std;

Client::Client() {}

int Client::ConnectToServer(const char* serverName, const char* portNumber)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(serverName, portNumber, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(_sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(_sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	
	return 0;
}

// get sockaddr, IPv4 or IPv6:
void* Client::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Client::ReceiveMessage(char* message, int length)
{
	int numbytes;

	if ((numbytes = recv(_sockfd, message, length, 0)) == -1)
	{
	    perror("recv");
	    return 1; 
	}

	return numbytes;
}

int Client::ReceiveMatrix(char* matrix, int rowCount, int colCount)
{
	int numbytes = 0;
	
	for(int row = 0; row < rowCount; row++)
		numbytes += ReceiveMessage(matrix + colCount*row, colCount);
	
	return numbytes;
}

void Client::CloseConnection()
{
	close(_sockfd);
}
