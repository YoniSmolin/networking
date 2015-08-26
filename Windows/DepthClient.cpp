/*
* DepthClient.cpp -- Kinect depth data client
*/

#include <opencv2/highgui/highgui.hpp>
#include <conio.h>
#include <iostream>

#include "Client.h" // networking class
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
	uchar imageBuffer1[ROWS*COLS], imageBuffer2[ROWS*COLS]; // we need two buffers because the server sends the compressed difference w.r.t previous frame
	
	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	cout << "Connected to server successfully" << endl;

	namedWindow("Client");
		
	Timer totalTime("Network + Processing", FRAMES_BETWEEN_TELEMETRY_MESSAGES);

	uchar* current = imageBuffer1;
	uchar* previous = imageBuffer2;

	bool firstRound = true;

	while (1)
	{
		totalTime.Start(); 
		
		int numBytes;
		if (firstRound)
		{
			numBytes = client.ReceiveMatrix((char*)current, ROWS, COLS);
			firstRound = false;
		}
		else
			numBytes = client.ReceiveCompressed(previous, current, ROWS, COLS);

		if (numBytes == 0) break;

		Mat image = Mat(ROWS, COLS, CV_8UC1, current);
		imshow("Client", image);
		waitKey(1);

		// swap the buffers
		uchar* tmp = previous;
		previous = current;
		current = tmp;

		totalTime.Stop(numBytes);
	}

	client.CloseConnection();

	return 0;
}


