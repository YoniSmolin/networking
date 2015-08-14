/*
** Timer.h - header for a timing class 
*/
#ifndef FPS_TIMER_H
#define FPS_TIMER_H

#include <windows.h>
#include <string>

class Timer
{
public:
	Timer(char* name);

	void Start();
	void Stop();

private:
	const int WindowSize = 100;
	int _sampleCounter;
	LARGE_INTEGER _start, _end, _frequency;
	float _accumulated;
	
	std::string _name;
};

#endif