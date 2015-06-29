#pragma once
#include <thread>
#include <atomic>

/**Creates and attaches a console for text output.*/
class Console
{
	void* handleIn, *handleOut, *handleErr;
	std::atomic<bool> loopEnabled;
	std::thread* loopThread;
	FILE* hfOut;
	FILE* hfIn;

public:
	/**Initalises varaiables to zero.*/
	Console();

	/**Enables the console.*/
	void enable();

	/**Disables the console.*/
	void disable();
};