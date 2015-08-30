/*
* DepthClient.cpp -- a TCP client class
*/

#include <ws2tcpip.h>
#include <stdio.h>
#include <stdexcept> // exceptions

#include "DepthClient.h"

#define BYTES_PER_COMPRESSED_PIXEL 4
#define BYTES_IN_HEADER 3
#define BITS_IN_BYTE 8 
#define LAST_TWO_LSBS 0x03
#define FIRST_MSB  0x80

#pragma region Constructors and Distructors

DepthClient::DepthClient(int rowCount, int colCount) : _colCount(colCount), _rowCount(rowCount), _expectingFirstFrame(true)
{
	_currentFrame = new uchar[rowCount * colCount];
	_compressedImageBuffer = new char[rowCount * colCount * BYTES_PER_COMPRESSED_PIXEL];
}

DepthClient::~DepthClient()
{
	if (_usingCompression)
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
		int numBytesReceived = waitUntilReceived((char*)&_usingCompression, sizeof(bool));
		if (numBytesReceived != sizeof(bool))
			return 0;
	}
	
	if (_usingCompression)
		return ReceiveMatrixCompressed();
	else
		return waitUntilReceived((char*)_currentFrame, _rowCount * _colCount);
}

int DepthClient::ReceiveMatrixCompressed()
{
	int numBytesRecieved = 0;

	if (_expectingFirstFrame)
	{
		numBytesRecieved = waitUntilReceived((char*)_currentFrame, _rowCount * _colCount); // first packet is delivered without compression...
		_expectingFirstFrame = false;
	}
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

#pragma endregion

#pragma region setters and getters

uchar* DepthClient::GetLatestFrame()
{
	return _currentFrame;
}

#pragma endregion

