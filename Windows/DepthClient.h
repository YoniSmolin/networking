/*
** DepthClient.h - a header file for the server class
*/

#ifndef DEPTHCLIENT_H
#define DEPTHCLIENT_H

#include "Client.h"

using namespace std;

typedef unsigned char uchar;

class DepthClient : public Client
{
	int _rowCount, _colCount;
	char* _compressedImageBuffer;
	uchar* _currentFrame;
	bool _usingCompression, _expectingFirstFrame;

public:
	DepthClient(string name, int rowCount, int colCount);

	int ReceiveMatrix(); // no need to allocate memory for matrix
	uchar* GetLatestFrame();
		
	~DepthClient();

private:
	int ReceiveMatrixCompressed();
	string _name;
};

#endif