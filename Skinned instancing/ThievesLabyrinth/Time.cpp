#pragma once
#include "Time.h"


clock_t CTime::tTimeStart;
clock_t CTime::tTimeCurrent;
double CTime::dDelta;

CTime::CTime()
{
	dDelta = 0.0;
	tTimeStart = clock();
}


CTime::~CTime()
{

}

// Updates the time and returns the
// difference in time in seconds
// since the last update call
void CTime::Update()
{
	static double dLast = 0.0;
	tTimeCurrent = clock();
	double dDiff = double(tTimeCurrent - tTimeStart) / CLOCKS_PER_SEC;
	dDelta = dDiff - dLast;
	dLast = dDiff;
}

double CTime::GetDelta()
{
	return dDelta;
}
