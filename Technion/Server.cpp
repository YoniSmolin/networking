/*
 ** Server.cpp - class implementation
 */

#include "Server.hpp" 

Server::Server(const char* portNumber)
{ 
	struct addrinfo hints, *servinfo, *p; 
	int yes=1; 
	int rv;

	memset(&hints, 0, sizeof hints); 
	hints.ai_family = AF_UNSPEC; // either IPv4 or IPv6 hints.ai_socktype = SOCK_STREAM; // TCP stream
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, portNumber, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(_sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(_sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL) 
	{
		fprintf(stderr, "server: failed to bind\n"); 
		exit(1);
	}
}
