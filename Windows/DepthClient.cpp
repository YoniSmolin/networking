/*
* DepthClient.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>
#include <stdexcept> // exceptions
#include <vector>    // for the received PNG

#include "DepthClient.h"

#define BYTES_PER_COMPRESSED_PIXEL 4
#define BYTES_IN_HEADER 3
#define BITS_IN_BYTE 8 
#define LAST_TWO_LSBS 0x03
#define FIRST_MSB  0x80

#define NO_COMPRESSION 0
#define DELTA_COMPRESSION 1
#define PNG_COMPRESSION 2

using namespace cv;

#pragma region Constructors and Distructors

DepthClient::DepthClient(string name, int rowCount, int colCount) : Client(name), _colCount(colCount), _rowCount(rowCount), _expectingFirstFrame(true)
{
	_currentFrame = new uchar[rowCount * colCount];
	_compressedImageBuffer = new char[rowCount * colCount * BYTES_PER_COMPRESSED_PIXEL];
}

DepthClient::~DepthClient()
{
	if (_compressionType == DELTA_COMPRESSION || _compressionType == PNG_COMPRESSION)
	{
		delete[] _compressedImageBuffer;
		delete[] _currentFrame;
	}
}

#pragma endregion

#pragma region send and recieve methods

int DepthClient::ReceiveMatrix()
{	
	if (_expectingFirstFrame)
	{
		int expectedLength = sizeof(_compressionType);
		int numBytesReceived = waitUntilReceived((char*)&_compressionType, expectedLength);
		if (numBytesReceived != expectedLength)
			return 0;
	}
	
	switch (_compressionType)
	{
	case NO_COMPRESSION:	return waitUntilReceived((char*)_currentFrame, _rowCount * _colCount);
	case DELTA_COMPRESSION: return ReceiveMatrixCompressedWithDelta();
	case PNG_COMPRESSION:   return ReceiveMatrixCompressedWithPNG();
	default:				throw exception("A bad compression type was received");
	}

	_expectingFirstFrame = false;
}

int DepthClient::ReceiveMatrixCompressedWithDelta()
{
	int numBytesRecieved = 0;

	if (_expectingFirstFrame)
		numBytesRecieved = waitUntilReceived((char*)_currentFrame, _rowCount * _colCount); // first packet is delivered without compression...
	else
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

		int indexCompressed = 0;
		int shift = 0;
		while (indexCompressed < compressedLength)
		{
			uchar firstByte = (uchar)_compressedImageBuffer[indexCompressed++];
			uchar secondByte = (uchar)_compressedImageBuffer[indexCompressed++];
			uchar thirdByte = (uchar)_compressedImageBuffer[indexCompressed++];
			int fourthByte = _compressedImageBuffer[indexCompressed++]; // fourth byte is signed

			int imageIndex = firstByte + (secondByte << BITS_IN_BYTE) + ((thirdByte & LAST_TWO_LSBS) << BITS_IN_BYTE * 2);
			bool isNegative = (thirdByte & FIRST_MSB) == FIRST_MSB; // meaning - the MSB is '1'
			if (isNegative)
				_currentFrame[imageIndex] -= fourthByte;
			else
				_currentFrame[imageIndex] += fourthByte;
		}

		numBytesRecieved = compressedLength + BYTES_IN_HEADER;
	}

	return numBytesRecieved;
}

int DepthClient::ReceiveMatrixCompressedWithPNG()
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
		Mat currentFrameMat = Mat(_rowCount, _colCount, CV_8UC1, _currentFrame); 
		imdecode(receivedPNG, CV_LOAD_IMAGE_ANYDEPTH, &currentFrameMat); // how does openCV know that receivedPNG contains a compressed PNG image ? by its content.

		return compressedLength + BYTES_IN_HEADER;
}

#pragma endregion

#pragma region setters and getters

const uchar* DepthClient::GetLatestFrame()
{
	return _currentFrame;
}

const Mat DepthClient::GetLatestFrameMat()
{
	return Mat(_rowCount, _colCount, CV_8UC1, _currentFrame);
}
#pragma endregion

