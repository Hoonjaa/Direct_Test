#include "stdafx.h"
#include "Timer.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Init()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceFrequency))
	{
		HardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&LastTime);
		TimeScale = 1.0 / PerformanceFrequency;
	}
	else
	{
		HardwareHasPerformanceCounter = FALSE;
		LastTime = ::timeGetTime();
		TimeScale = 0.001;
	}

	SampleCount = 0;
	CurrentFrameRate = 0;
	FramesPerSecond = 0;
	FPSTimeElapsed = 0.0;
	TimeElapsed = 0.0;
}

void Timer::Update(double dLockFPS)
{
	if (HardwareHasPerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	}
	else
	{
		CurrentTime = ::timeGetTime();
	}

	double dTimeElapsed = (CurrentTime - LastTime) * TimeScale;

	if (dLockFPS > 0.0)
	{
		while (dTimeElapsed < (1.0 / dLockFPS))
		{
			if (HardwareHasPerformanceCounter)
			{
				::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
			}
			else
			{
				CurrentTime = ::timeGetTime();
			}
			dTimeElapsed = (CurrentTime - LastTime) * TimeScale;
		}
	}

	LastTime = CurrentTime;

	if (fabs(dTimeElapsed - TimeElapsed) < 1.0)
	{
		::memmove(&FrameTime[1], FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(double));
		FrameTime[0] = dTimeElapsed;
		if (SampleCount < MAX_SAMPLE_COUNT) SampleCount++;
	}

	FramesPerSecond++;
	FPSTimeElapsed += dTimeElapsed;
	if (FPSTimeElapsed > 1.0)
	{
		CurrentFrameRate = FramesPerSecond;
		FramesPerSecond = 0;
		FPSTimeElapsed = 0.0;
	}

	TimeElapsed = 0.0;
	for (ULONG i = 0; i < SampleCount; i++) TimeElapsed += FrameTime[i];
	if (SampleCount > 0) TimeElapsed /= SampleCount;

#ifdef _DEBUG
	if (TimeElapsed > (1. / 60.))
		TimeElapsed = (1. / 60.);
#endif
}

void Timer::Reset()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	LastTime = nPerformanceCounter;
	CurrentTime = nPerformanceCounter;

	Stopped = false;
}