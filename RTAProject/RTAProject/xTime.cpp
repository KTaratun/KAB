#include "xTime.h"


XTime::XTime(unsigned char samples, double smoothFactor)
{
	ZeroMemory(&localStack, sizeof(THREAD_DATA));
	localStack.numSamples = max(1, samples);// one sample is minimum
	localStack.blendWeight = smoothFactor;
	localStack.threadID = GetCurrentThreadId();
	localStack.threadDelay = 0; // no delay by default
	Restart();
}

void XTime::Restart()
{
	QueryPerformanceFrequency(&localStack.frequency);

	localStack.deltaTime = localStack.totalTime = localStack.smoothDelta = 0.0;
	localStack.signalCount = 0;

	QueryPerformanceCounter(&localStack.start);
	localStack.signals[localStack.signalCount++] = localStack.start;
}

double XTime::TotalTime()
{
	return localStack.totalTime;
}

double XTime::TotalTimeExact()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now); //Time Right Now
	LONGLONG elapsed = now.QuadPart - localStack.start.QuadPart; //Time elapsed since start
	return double(elapsed) / double(localStack.frequency.QuadPart); //In Seconds
}

double XTime::Delta()
{
	return localStack.deltaTime;
}

double XTime::SmoothDelta()
{
	return localStack.smoothDelta;
}


void XTime::Signal()
{
	memmove_s(localStack.signals + 1u, sizeof(LARGE_INTEGER) * 255, localStack.signals, sizeof(LARGE_INTEGER) * localStack.numSamples);

	QueryPerformanceCounter(localStack.signals);
	localStack.signalCount = min(localStack.signalCount + 1, 255);

	localStack.totalTime = double((*localStack.signals).QuadPart - localStack.start.QuadPart) / double(localStack.frequency.QuadPart);
	localStack.deltaTime = double(localStack.signals[0].QuadPart - localStack.signals[1].QuadPart) / double(localStack.frequency.QuadPart);

	double totalWeight = 0, runningWeight = 1;
	LONGLONG totalValue = 0, sampleDelta;

	for (unsigned char i = 0; i < min(localStack.numSamples, localStack.signalCount-1); ++i)
	{
		sampleDelta = localStack.signals[i].QuadPart - localStack.signals[i + 1].QuadPart;
		totalValue += LONGLONG(sampleDelta * runningWeight);
		totalWeight += runningWeight;
		runningWeight *= localStack.blendWeight;
	}
	localStack.smoothDelta = (totalValue / totalWeight) / double(localStack.frequency.QuadPart);
}

void XTime::Throttle(double targetHz, bool smoothing)
{
	if (targetHz)
	{
		// what delta is desired for this thread of execution?
		double desiredDelta = 1.0 / targetHz;
		double currentDelta = (smoothing) ? localStack.smoothDelta : localStack.deltaTime;
		// how long did we previously sleep to acheive the currentDelta? (seconds)
		double prevDelay = localStack.threadDelay * 0.001;
		// how much time exists between desiredDelta and currentDelta? (seconds)
		double adjustDelta = desiredDelta - currentDelta;
		// tweak the previous delay by the adjustment factor
		double currDelay = prevDelay + adjustDelta;
		// update the thread delay to match the current delay as closely as possible.
		localStack.threadDelay = DWORD(max(0, currDelay * 1000));
		// only sleep if we need to slow down this thread
		if (currDelay > 0)
			Sleep(localStack.threadDelay);
	}
}