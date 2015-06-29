#pragma once
#include <Windows.h>
#include "immediateio.h"
#include<iostream>
/**Silly Visual Studio bug forces me to use system_clock ^^ */
//Recall that deltaTime = time per frame!

class Timer
{
private:
	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mStart;
	LARGE_INTEGER mEnd;
	float mFPS;
	float mDeltaTime;

	void computeFPS(float deltaTime)
	{
		mFPS = (deltaTime == 0) ? 0 : 1.f / deltaTime;
	}

	void computeDeltaTime()
	{
		//Time taken / precision per second
		mDeltaTime = (float)(((double)(mEnd.QuadPart - mStart.QuadPart)) / ((double)(mFrequency.QuadPart)));
	}
public:

	Timer()
	{
		mDeltaTime = mFPS = 0;
		ZeroMemory(&mStart, sizeof(mStart));
		ZeroMemory(&mEnd, sizeof(mEnd));
		QueryPerformanceFrequency(&mFrequency);
	}
	void startFrame()
	{
		QueryPerformanceCounter(&mStart);
	}

	void endFrame()
	{
		QueryPerformanceCounter(&mEnd);
		computeDeltaTime();
		computeFPS(mDeltaTime);
		static int k = 0;
		if (k++ % 100 == 0)
			std::cout << mFPS << "\n";
	}

	float deltaTime()
	{
		return mDeltaTime;
	}

	float fps()
	{
		//Time taken / precision per second
		return mFPS;
	}
};