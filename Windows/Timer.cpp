/*
** Timer.cpp - an implementation of a frame rate measuring class
*/
#include "Timer.h"
#include <stdio.h>

Timer::Timer(char* name, int windowSize) : _sampleCounter(0), _accumulatedTime(0), _accumulatedBytes(0), _name(name), _windowSize(windowSize), _windowsCounter(0), _sessionBandwidthSum(0)
{
	QueryPerformanceFrequency(&_frequency);
}

void Timer::Start()
{
	if (_sampleCounter == 0)
		QueryPerformanceCounter(&_start);

	_sampleCounter++;
}

void Timer::Stop(int numBytesMoved)
{
	_accumulatedBytes += numBytesMoved;
	 
	if (_sampleCounter == _windowSize)
	{
		QueryPerformanceCounter(&_end);
		float accumulatedTime = _end.QuadPart - _start.QuadPart;
		float cycle = (accumulatedTime / _frequency.QuadPart) / _windowSize;
		float bandwidth = (_accumulatedBytes * 8 / (accumulatedTime / _frequency.QuadPart)) / (1 << 20); // Mbps
		printf("%s : Rate - %2.1f [Hz], Cycle - %2.1f [mSec]\n", _name.c_str(), 1/cycle, 1000 * cycle);
		printf("%s : Bandwidth - %2.1f [Mbps]\n", _name.c_str(), bandwidth);

		_sessionBandwidthSum += bandwidth;
 		_accumulatedBytes = 0;
		_sampleCounter = 0;
		_windowsCounter++;
	}
}

float Timer::AverageBandwidth()
{
	if (_windowsCounter == 0)
		return 0;

	return _sessionBandwidthSum / _windowsCounter;
}