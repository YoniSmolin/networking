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
	Timer(char* name, int windowSize);

	void  Start();
	void  Stop(int numBytesMoved);
	float AverageBandwidth(); // returns the average bit rate during the session, in units of [Mbps]

private:
	int _windowSize;
	int _sampleCounter;
	int _windowsCounter;
	LARGE_INTEGER _start, _end, _frequency;
	float _accumulatedTime;
	float _sessionBandwidthSum;
	int  _accumulatedBytes;
	 
	std::string _name;
};

#endif