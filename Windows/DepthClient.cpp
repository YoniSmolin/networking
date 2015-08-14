/*
* DepthClient.cpp -- Kinect depth data client
*/

#include <opencv2/highgui/highgui.hpp>
#include <conio.h>
#include <iostream>

#include "Client.h"
#include "Timer.h"

using namespace std;
using namespace cv;

#define PORT "3490"
#define SERVER_NAME "192.168.137.3"

#define ROWS 424
#define COLS 512

#define NUM_FRAMES 1000

int main(int argc, char** argv)
{
	uchar imageArray[ROWS*COLS];

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	cout << "Connected to server successfully" << endl;

	namedWindow("Client");
		
	Timer timer;

	while (1)
	{
		timer.Start(); 

		int numbytes = client.ReceiveMatrix((char*)imageArray, ROWS, COLS);

		if (numbytes == 0) break;

		Mat image = Mat(ROWS, COLS, CV_8UC1, imageArray);
		imshow("Client", image);
		waitKey(1);

		timer.Stop();
	}

	client.CloseConnection();
	cout << "Connection closed" << endl;

	//cout << "Press any key to continue" << endl;
	//while (!_kbhit());

	return 0;
}
