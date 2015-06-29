#include <Windows.h>
#include "Console.h"
#include <io.h>
#include <fcntl.h>

void ConsoleLoop(std::atomic<bool>* loopEnabled, HWND hwnd)
{
	using namespace std;
	string str;
	str.resize(256);
	LPDWORD read = new DWORD;

	while (*loopEnabled)
	{
		if (WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 1) != WAIT_OBJECT_0)
		{
			ReadConsole(GetStdHandle(STD_INPUT_HANDLE), (char*)&str[0], 255 * sizeof(TCHAR), read, NULL);

			if (str == "quit")
			{
				PostMessage(hwnd, WM_QUIT, 0, 0);
				return;
			}
		}
	}
	delete read;
}

Console::Console()
{
	hfIn = 0;
	hfOut = 0;
	loopEnabled = false;
}

void Console::enable()
{
	AllocConsole();

	handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handleOut, _O_TEXT);
	hfOut = _fdopen(hCrt, "w");
	setvbuf(hfOut, NULL, _IONBF, 0); //Can assign custom buffer here! Currently no buffering (_IONBF)
	*stdout = *hfOut;

	handleIn = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handleIn, _O_TEXT);
	hfIn = _fdopen(hCrt, "r");
	setvbuf(hfIn, NULL, _IOLBF, 256);
	*stdin = *hfIn;

	loopEnabled = true;
	//loopThread = new std::thread(ConsoleLoop, &loopEnabled);

}

void Console::disable()
{
	loopEnabled = false;
	if (loopThread)
	{
		//	loopThread->join();
		//	delete loopThread;
	}
	FreeConsole();
}