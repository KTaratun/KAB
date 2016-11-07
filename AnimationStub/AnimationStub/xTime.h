#pragma once
#include "Windows.h"

class XTime
{
	struct THREAD_DATA
	{
		LARGE_INTEGER signals[256], frequency, start;
		double totalTime, deltaTime, smoothDelta, blendWeight;
		unsigned int threadID, threadDelay;
		unsigned char signalCount, numSamples;
	}localStack;

public:

	XTime(unsigned char samples = 10, double smoothFactor = 0.75);

	void Restart();
	double TotalTime();
	double TotalTimeExact();
	double Delta();
	double SmoothDelta();
	void Signal();
	void Throttle(double targetHz, bool smoothing = false);

};
