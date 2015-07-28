#include "Server.hpp"

#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	Server server("3490");
	//server.WaitForClient();
	//server.Transmit(data);
	cout << "Successfully initialized server" << endl;
	return 0;
}
