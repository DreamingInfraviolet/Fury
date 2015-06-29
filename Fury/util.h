#pragma once
#include<string>
#include <sstream>
#include "enum.h"

template<class T>
void deletePtr(T*& ptr)
{
	if(ptr)
		delete ptr;
	ptr = NULL;
}

std::wstring stringToWstring(const std::string& str);

std::string stringFromStrLen(const char* str, size_t length);

template<class T> std::string toString(T num, Enum::Format format = Enum::Dec)
{
	std::ostringstream oss;

	switch (format)
	{
	case Enum::Dec:
		oss << num;
		break;
	case Enum::Hex:
		oss << std::hex << num;
		break;
	}

	return oss.str();
}

std::string toLowerCase(const std::string& str);