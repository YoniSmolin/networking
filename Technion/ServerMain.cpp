#include "Server.hpp"

#include <iostream>
#include <string.h>

using namespace std;

#define PORT "3490"

int main(int argc, char** argv)
{
	string message = "Hey there, client !";
	Server server(PORT);
	cout << "Successfully initialized server" << endl;
	server.WaitForClient();
	server.SendMessage(message.c_str(), message.length());
	server.CloseConnection();
	cout << "Connection closed" << endl;
	return 0;
}
