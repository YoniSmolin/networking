/*
** Timer.h - header for a timing class 
*/
#ifndef FPS_TIMER_H
#define FPS_TIMER_H

#include <windows.h>

class Timer
{
public:
	Timer();

	void Start();
	void Stop();

private:
	const int WindowSize = 100;
	int _sampleCounter;
	LARGE_INTEGER _start, _end, _frequency;
	float _accumulated;

};

#endif