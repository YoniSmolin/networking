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

		int ConnectToServer(const char* serverName, const char* portNumber);
		int ReceiveMessage(char* message, int length);
		int ReceiveMatrix(char* matrix, int rowCount, int colCount);
	        void CloseConnection();		

	private:
		void sigchld_handler(int s);
		void* get_in_addr(struct sockaddr *sa);

};

#endif
