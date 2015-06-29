
#include "system.h"
#include "Console.h"

#include <cstdlib>

#pragma warning(suppress: 28251)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmndLine, int nCmdShow)
{
	Console console;
	console.enable();
	System* system = System::createSystem();
	if (!system)
		return 1;
	if (!system->initialise(hInstance, lpCmndLine, nCmdShow))
		return 2;
	bool result =  system->run();
	system->shutdown();
	console.disable();
	return (result ? 0:3);
}