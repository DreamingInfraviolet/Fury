#pragma once
#include<vector>
#include<string>
class GraphicsInfo
{
public:
	GraphicsInfo(){}

	struct GraphicsMode
	{
		int width;
		int height;
		float refreshRate;
		unsigned refreshRateNumerator;
		unsigned refreshRateDenominator;
	};

	struct Card
	{
		std::wstring description;
		unsigned vendorID;
		unsigned deviceID;
		unsigned revision;
		long long videoMemory; //GPU memory
		long long systemMemory; //RAM reserved for GPU
		long long sharedSystemMemory; //RAM allowed for the GPU to use
	};

	std::vector<GraphicsMode> graphicsModes;
	Card card;
};