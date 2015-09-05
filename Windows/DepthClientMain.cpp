/*
* DepthClient.cpp -- Kinect depth data client
*/

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "DepthClient.h" // networking class
#include "Timer.h"  // telemetry class

using namespace std;
using namespace cv;

#define PORT "3490" // randomly chosen
#define SERVER_NAME "JetsonBoard1.local" // JetsonBoardi.local where i stands for the index of the board (written in red on the board itself)

#define ROWS 424 // depth image 1st dimension
#define COLS 512 // depth image 2nd dimension

#define FRAMES_BETWEEN_TELEMETRY_MESSAGES 30

#define MAX_DISTANCE_MM 4500.0f

int main(int argc, char** argv)
{
	DepthClient client(ROWS, COLS);
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	cout << "Connected to server successfully" << endl;

	namedWindow("Client"); // OpenCV windows
		
	Timer telemetry("Network + Processing", FRAMES_BETWEEN_TELEMETRY_MESSAGES);

	uchar* imageDataPointer = NULL;
	int numBytes = 0;

	while (1)
	{
		telemetry.Start(); 
		
		numBytes = client.ReceiveMatrix();
		imageDataPointer = client.GetLatestFrame();

		if (numBytes == 0) break; // Server finished delivering data

		Mat image = Mat(ROWS, COLS, CV_8UC1, imageDataPointer);
		imshow("Client", image);
		waitKey(1);

		telemetry.Stop(numBytes);
	}

	client.CloseConnection();

	printf("Average bandwidth for this session was: %2.1f [Mbps]\n", telemetry.AverageBandwidth());

	return 0;
}


