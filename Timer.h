#pragma once
const ULONG MAX_SAMPLE_COUNT = 50;

class Timer
{
private:
	bool HardwareHasPerformanceCounter;
	double TimeScale;
	double TimeElapsed;
	__int64 CurrentTime;
	__int64 LastTime;
	__int64 PerformanceFrequency;

	double FrameTime[MAX_SAMPLE_COUNT];
	ULONG SampleCount;

	unsigned long CurrentFrameRate;
	unsigned long FramesPerSecond;
	double FPSTimeElapsed;

	bool Stopped;

public:
	Timer();
	~Timer();

	void Init();
	void Update(double dLockFPS = 0.0);
	void Reset();

	double getDT() { return TimeElapsed; }
	float getfDT() { return (float)TimeElapsed; }
};

