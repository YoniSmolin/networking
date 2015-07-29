#include "Server.hpp"

#include <iostream>
#include <string.h>

using namespace std;

#define PORT "3490"

#define MESSAGE "1111222233334444"
#define ROWS 4
#define COLS 4

int main(int argc, char** argv)
{
	string message = MESSAGE;
	Server server(PORT);
	cout << "Successfully initialized server" << endl;
	server.WaitForClient();
	//server.SendMessage(message.c_str(), message.length());
	server.SendMatrix(message.c_str(), ROWS, COLS);
	server.CloseConnection();
	cout << "Connection closed" << endl;
	return 0;
}
