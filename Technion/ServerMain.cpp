#include "Server.hpp"

#include <iostream>
#include <string.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

#define PORT "3490"

#define ROWS 512
#define COLS 512

int main(int argc, char** argv)
{
	uchar imageArray[ROWS*COLS];
	for(int i = 0; i < ROWS; i++)
		for(int j = 0; j<COLS; j++)
			imageArray[i*COLS + j] = (uchar)(255.f * j / COLS);

	Server server(PORT);
	cout << "Successfully initialized server" << endl;
	server.WaitForClient();
	server.SendMatrix((char*)imageArray, ROWS, COLS);
	server.CloseConnection();
	cout << "Connection closed" << endl;
	return 0;
}
