/*
** Timer.cpp - an implementation of a frame rate measuring class
*/
#include "Timer.h"
#include <stdio.h>

Timer::Timer(char* name) : _sampleCounter(0), _accumulated(0), _name(name)
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
		float cycle = _accumulated / WindowSize;
		printf("%s : Cycle - %2.1f [mSec], Rate - %2.1f [Hz] \n", _name.c_str(), 1000 * cycle / _frequency.QuadPart, _frequency.QuadPart / cycle);
 		_accumulated = 0;
		_sampleCounter = 0;
	}
}