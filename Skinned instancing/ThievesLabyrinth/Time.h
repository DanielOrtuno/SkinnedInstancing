#pragma once
#include <time.h>
#include <chrono>
//Time
class CTime
{
	static clock_t tTimeStart, tTimeCurrent;
	static double dDelta;
public:
	CTime();

	// Updates the time and returns the
	// difference in time in seconds
	// since the last update call
	static void Update();

	// Get the difference of time in seconds
	static double GetDelta();

	~CTime();
};

