#include "Server.hpp"

#include <iostream>
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>

using namespace std;

#define PORT "3490"

#define ROWS 512
#define COLS 512

#define NUM_FRAMES 1000
#define FRAME_CYCLE 1000

int main(int argc, char** argv)
{
	uchar imageArray[ROWS*COLS];

	Server server(PORT);
	cout << "Successfully initialized server" << endl;
	server.WaitForClient();

	// sending loop
	for(int frame = 0; frame < NUM_FRAMES; frame ++)
	{
		for(int i = 0; i < ROWS; i++)
			for(int j = 0; j<COLS; j++)
				imageArray[i*COLS + ((j + frame) % COLS)] = (uchar)(255.f * j / COLS);
		server.SendMatrix((char*)imageArray, ROWS, COLS);
		usleep(FRAME_CYCLE);
	}

	server.CloseConnection();
	cout << "Connection closed" << endl;
	return 0;
}
