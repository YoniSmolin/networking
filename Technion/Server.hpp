/*
 ** server.hpp - a header file for the server class 
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Server
{
	int _sockfd;
	public:
		Server(const char* portNumber);
};

#endif
