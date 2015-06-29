#include<string>
#include <windows.h>
#include "util.h"
#include <iostream>
#include <string>



void Error(std::string err)
{
	MessageBox(NULL, (std::wstring(L"Error: ") + stringToWstring(err)).c_str(), L"Error", MB_OK | MB_ICONERROR);
}

void Warning(std::string err)
{
	MessageBox(NULL, (std::wstring(L"Warning: ") + stringToWstring(err)).c_str(), L"Warning", MB_OK | MB_ICONWARNING);
}

void Inform(std::string inf)
{
	std::cout << ":- " << inf << "\n";
}

void ErrorSafe(std::string err)
{
	std::cout << "ERROR: " << err << "\n";
}