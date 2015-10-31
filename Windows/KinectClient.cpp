/*
* KinectClient.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>
#include <stdexcept> // exceptions
#include <vector>    // for the received PNG

#include "KinectClient.h"

#define BYTES_PER_COMPRESSED_PIXEL 2
#define BYTES_IN_HEADER 3
#define BITS_IN_BYTE 8 
#define LAST_TWO_LSBS 0x03
#define FIRST_MSB  0x80

#define NO_COMPRESSION 0
#define DELTA_COMPRESSION 1
#define PNG_COMPRESSION 2

using namespace cv;

#pragma region Constructors and Distructors

KinectClient::KinectClient(string name, int rowCount, int colCount) : Client(name), _colCount(colCount), _rowCount(rowCount)
{
	_currentFrame = new uchar[rowCount * colCount * 2];
	_compressedImageBuffer = new char[rowCount * colCount * BYTES_PER_COMPRESSED_PIXEL];
}

KinectClient::~KinectClient()
{
		delete[] _compressedImageBuffer;
		delete[] _currentFrame;
}

#pragma endregion

#pragma region send and recieve methods

int KinectClient::ReceiveMatrixCompressedWithPNG()
{
		uchar header[BYTES_IN_HEADER];
		int totalReceived = waitUntilReceived((char*)header, BYTES_IN_HEADER);
		if (totalReceived < BYTES_IN_HEADER) // totalReceived will be less than BYTES_IN_HEADER only if server has closed connection
			return 0;

		// reconstruct the length of the compressed image from the header
		int compressedLength = header[0];
		compressedLength += header[1] << BITS_IN_BYTE;
		compressedLength += header[2] << 2 * BITS_IN_BYTE;

		// reconstruct the image itself
		totalReceived = waitUntilReceived(_compressedImageBuffer, compressedLength);
		if (totalReceived < compressedLength) // totalReceived will be less than BYTES_IN_HEASER only if server has closed connection
			return 0;
		
		vector<uchar> receivedPNG(_compressedImageBuffer, _compressedImageBuffer + compressedLength);
		Mat currentFrameMat = Mat(_rowCount, _colCount, CV_16UC1, _currentFrame); 
		imdecode(receivedPNG, CV_LOAD_IMAGE_ANYDEPTH, &currentFrameMat); // how does openCV know that receivedPNG contains a compressed PNG image ? by its content.

		return compressedLength + BYTES_IN_HEADER;
}

#pragma endregion

#pragma region setters and getters

const uchar* KinectClient::GetLatestFrame()
{
	return _currentFrame;
}

const Mat KinectClient::GetLatestFrameMat()
{
	return Mat(_rowCount, _colCount, CV_16UC1, _currentFrame);
}
#pragma endregion

