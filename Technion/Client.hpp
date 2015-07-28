/*
 ** Client.hpp - a header file for the server class 
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
	int _sockfd; 

	public:
		Client();

		void ConnectToServer(const char* serverName, const char* portNumber);

	private:
		void sigchld_handler(int s);
		void* get_in_addr(struct sockaddr *sa);

};

#endif
