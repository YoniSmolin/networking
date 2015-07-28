#include "Client.hpp"

#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer("localhost", "3490");
	return 0;
}
