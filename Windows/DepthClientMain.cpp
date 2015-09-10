/*
* DepthClient.cpp -- Kinect depth data client
*/

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "DepthClient.h" // networking class
#include "Timer.h"  // telemetry class

#include <windows.h>  // multithreading
#include <process.h>  // multithreading
//#include <Synchapi.h> // barriers

using namespace std;
using namespace cv;

#define PORT "3490" // randomly chosen
#define SERVER_NAME_HEADER "JetsonBoard" // JetsonBoardi.local where i stands for the index of the board (written in red on the board itself)
#define SERVER_NAME_TAIL   ".local"

#define ROWS 424 // depth image 1st dimension
#define COLS 512 // depth image 2nd dimension

#define FRAMES_BETWEEN_TELEMETRY_MESSAGES 30

#define MAX_DISTANCE_MM 4500.0f
 
#define THREAD_BARRIER_SPIN_COUNT -1 // the number of times a thread will attempt to check the barrier state before it blocks (-1 corresponds to a system defalut value of 2k)

LPSYNCHRONIZATION_BARRIER barrier; // a GLOBAL thread barrier
bool FastestThreadFinished = false;

unsigned __stdcall KinectClientThreadFunction(void* kinectIndex);

int main(int argc, char** argv)
{
	#pragma region process input arguments
		if (argc > 2)
		{
			cout << "usage: " << argv[0] << " -n" << endl << "(n stands for the number of clients to launch)" << endl;
			return 1;
		}
		int threadCount = atoi(argv[1]);
	#pragma endregion

	#pragma region initialize synchronziation barrier	
		barrier = (LPSYNCHRONIZATION_BARRIER) malloc(sizeof(SYNCHRONIZATION_BARRIER));
		BOOL result = InitializeSynchronizationBarrier(barrier, threadCount, -1);
		if (result != TRUE)
		{
			cout << "Failed to initalize thread barrier" << endl << "last error: " << GetLastError() << endl;
			return 1;
		}
	#pragma endregion

	#pragma region launch threads
		HANDLE* handlesToThreads = new HANDLE[threadCount];//(HANDLE*) malloc(sizeof(HANDLE) * threadCount);
		int* threadIndices = new int[threadCount];

		for (int i = 0; i < threadCount; i++)
		{
			threadIndices[i] = i+1; // I don't like to enumerate stuff from 0
			handlesToThreads[i] = (HANDLE)_beginthreadex(NULL, 0, &KinectClientThreadFunction, &threadIndices[i], CREATE_SUSPENDED, NULL);
		}

		for (int i = 0; i < threadCount; i++)
		{
			ResumeThread(handlesToThreads[i]);
		}
	#pragma endregion

	#pragma region wait for threads to terminate
		for (int i = 0; i < threadCount; i++)
		{
			WaitForSingleObject(handlesToThreads[i], INFINITE);
		}
		DeleteSynchronizationBarrier(barrier);
	#pragma endregion
}

unsigned __stdcall KinectClientThreadFunction(void* kinectIndex)
{
	int index = *((int*)kinectIndex);
	char clientIndex[3]; 

	_itoa_s(index, clientIndex, 3, 10); // 10 is the radix

	DepthClient client(string(clientIndex), ROWS, COLS);
	cout << "Initialized client #" << clientIndex << " successfully" << endl;
	string serverName = string(SERVER_NAME_HEADER) + string(clientIndex) + string(SERVER_NAME_TAIL);
	client.ConnectToServer(index == 1 ? "132.68.63.164":"132.68.56.152", PORT);    //serverName.c_str(), PORT); // 
	cout << "Connected to server #" << clientIndex << " successfully" << endl;

	string windowName = string("Client #") + string(clientIndex);
	namedWindow(windowName); // OpenCV windows

	Timer telemetry(string("Kinect #") + string(clientIndex), FRAMES_BETWEEN_TELEMETRY_MESSAGES);

	uchar* receivedImageData = NULL;
	int numBytes = 0;

	while (1)
	{
		telemetry.Start();

		numBytes = client.ReceiveMatrix(); // after it is received the matrix is stored internally in the client
		receivedImageData = client.GetLatestFrame(); // to obtain the received matrix...

		if (numBytes == 0) FastestThreadFinished = true;

		if (numBytes > 0)
		{
			Mat image = Mat(ROWS, COLS, CV_8UC1, receivedImageData);
			imshow(windowName, image);
			waitKey(1);

			telemetry.Stop(numBytes);
		}

		EnterSynchronizationBarrier(barrier, 0); // 0 means no flags

		if (FastestThreadFinished)
			break;
	}

	client.CloseConnection();

	printf("Average bandwidth for Kinect #%s was: %2.1f [Mbps]\n", clientIndex, telemetry.AverageBandwidth());

	return 0;
}