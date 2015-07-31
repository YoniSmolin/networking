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

#define ROWS 512 
#define COLS 512

int main(int argc, char** argv)
{
	uchar imageArray[ROWS*COLS];

	Client client;
	cout << "Initialized client successfully" << endl;
	client.ConnectToServer(SERVER_NAME, PORT);
	client.ReceiveMatrix((char*)imageArray, ROWS, COLS); 
	client.CloseConnection();
	cout << "Connection closed" << endl;
	
	Mat image = Mat(ROWS, COLS, CV_8UC1, imageArray);
	imshow("ramp", image);
	waitKey();

	return 0;
}
