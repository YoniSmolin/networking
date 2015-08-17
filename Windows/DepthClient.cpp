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

#define MAX_DISTANCE_MM 4500.0f

int main(int argc, char** argv)
{
	uchar imageBuffer1[ROWS*COLS], imageBuffer2[ROWS*COLS];
	
	Client client(ROWS, COLS);
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	cout << "Connected to server successfully" << endl;

	namedWindow("Client");
		
	Timer totalTime("Network + Processing");
	Timer networkTime("Network alone");

	uchar* current = imageBuffer1;
	uchar* previous = imageBuffer2;

	while (1)
	{
		totalTime.Start(); 
		networkTime.Start();
		
		int numbytes = client.ReceiveCompressed(previous, current, ROWS, COLS);
		
		networkTime.Stop();

		if (numbytes == 0) break;

		Mat image = Mat(ROWS, COLS, CV_8UC1, current);
		imshow("Client", image);
		waitKey(1);

		// swap the buffers
		uchar* tmp = previous;
		previous = current;
		current = tmp;

		totalTime.Stop();
	}

	client.CloseConnection();
	cout << "Connection closed" << endl;

	//cout << "Press any key to continue" << endl;
	//while (!_kbhit());

	return 0;
}


