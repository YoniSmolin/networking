/*
** Timer.cpp - an implementation of a frame rate measuring class
*/
#include "Timer.h"
#include <stdio.h>

Timer::Timer() : _sampleCounter(0), _accumulated(0)
{
	QueryPerformanceFrequency(&_frequency);
}

void Timer::Start()
{
	QueryPerformanceCounter(&_start);
	_sampleCounter++;
}

void Timer::Stop()
{
	QueryPerformanceCounter(&_end);
	_accumulated += _end.QuadPart - _start.QuadPart;

	if (_sampleCounter == WindowSize)
	{
		printf("FPS: %2.1f [Hz]\n", _frequency.QuadPart / (_accumulated / WindowSize));
 		_accumulated = 0;
		_sampleCounter = 0;
	}
}