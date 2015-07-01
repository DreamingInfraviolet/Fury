/*
=====================================================================

Book				: Learning Physics Modeling with PhysX (ISBN: 978-1-84969-814-6)
Author				: Krishna Kumar
Compiler used		: Visual C++ 2010 Express
PhysX SDK version	: 3.3.0
Source code name	: ch2_1_HelloPhysx
Reference Chapter	: Chapter-2: Basic Concepts

Description			: This example demonstrates the initialization, stepping and shutdown of PhysX SDK version 3.3.0
It is more like 'Hello World' program for PhysX SDK and contains minimal code. It mainly contains
three function which are as follows;

void InitPhysX();		//Initialize the PhysX SDK and create two actors.
void StepPhysX();		//Step PhysX simulation 300 times.
void ShutdownPhysX(); // Shutdown PhysX SDK

ConnectPVD();			//Function for the visualization of PhysX simulation (Optional)

This example code itself doesn't provide any implementation for rendering the PhysX objects.
However you can use PVD software to visualize the simulation. PVD can only be used in 'Debug' mode(configuration).
Please refer to last chapter (PhysX Visual Debugger) for more information.

=====================================================================
*/




#include "Console.h"
#include <Windows.h>
#include "system.h"
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

	bool result = system->run();
	system->shutdown();
	console.disable();
	return (result ? 0 : 3);

	


}


