/*
 * ClientMain.cpp -- client program
 */

#include "Client.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define PORT "3490"
#define SERVER_NAME "localhost"

#define ROWS 424
#define COLS 512

#define NUM_FRAMES 1000

int main(int argc, char** argv)
{
	uchar imageArray[ROWS*COLS];

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);

	namedWindow("ramp",1);

	// sending loop
	for(int frame = 0; frame < NUM_FRAMES; frame ++)
	{
		int numbytes = client.ReceiveMatrix((char*)imageArray, ROWS, COLS); 
		
		if (numbytes == 0) break;

		Mat image = Mat(ROWS, COLS, CV_8UC1, imageArray);
		imshow("ramp", image);
		waitKey(1);
	}

	client.CloseConnection();
	cout << "Connection closed" << endl;
	
	return 0;
}
