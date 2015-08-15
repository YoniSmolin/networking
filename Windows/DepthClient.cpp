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
	float imageArray[ROWS*COLS];
	Mat scaledDepth;

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	cout << "Connected to server successfully" << endl;

	namedWindow("Client");
		
	Timer totalTime("Network + Processing");
	Timer networkTime("Network alone");

	while (1)
	{
		totalTime.Start(); 
		networkTime.Start();
		
		int numbytes = client.ReceiveMatrix(imageArray, ROWS, COLS);
		
		networkTime.Stop();

		if (numbytes == 0) break;

		Mat image = Mat(ROWS, COLS, CV_32FC1, imageArray) / MAX_DISTANCE_MM;
		image.convertTo(scaledDepth, CV_8UC1, 255, 0);
		imshow("Client", scaledDepth);
		waitKey(1);

		totalTime.Stop();
	}

	client.CloseConnection();
	cout << "Connection closed" << endl;

	//cout << "Press any key to continue" << endl;
	//while (!_kbhit());

	return 0;
}
