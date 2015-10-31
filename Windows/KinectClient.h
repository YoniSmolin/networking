/*
** KinectClient.h - a header file for the server class
*/

#ifndef KinectClient_H
#define KinectClient_H

#include "Client.h"
#include <opencv2/highgui/highgui.hpp> // decoding the PNG image

using namespace std;
using namespace cv;

typedef unsigned char uchar;

class KinectClient : public Client
{
	int _rowCount, _colCount;
	char* _compressedImageBuffer;
	uchar* _currentFrame;

public:
	KinectClient(string name, int rowCount, int colCount);

	int ReceiveMatrixCompressedWithPNG(); // no need to allocate external memory for the output matrix - the output is written to a class member
	const uchar* GetLatestFrame();
	const Mat KinectClient::GetLatestFrameMat();

	~KinectClient();

private:
	string _name;
};

#endif