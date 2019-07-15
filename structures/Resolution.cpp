#include "Resolution.h"

bool ResolutionSort(stResolution A, stResolution B)
{
	return A.X < B.X;
}

stResolution GetCurrentResolution()
{
	stResolution CurrentResolution;
	CurrentResolution.X = GetSystemMetrics(SM_CXSCREEN);
	CurrentResolution.Y = GetSystemMetrics(SM_CYSCREEN);
	return CurrentResolution;
}

bool GetSupportedResolutions(std::vector<stResolution>* pResolutions)
{
	if(!pResolutions) return false;
	
	unsigned short MinBitsPerPixel = 32;
	unsigned short MinResolutionX = 800;
	
	stResolution Resolution;
	std::vector<stResolution>::iterator it;
	
	DEVMODE dm = {0};
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);
	for(int iModeNum = 0, k = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum ++)
	{
		Resolution.X = dm.dmPelsWidth;
		Resolution.Y = dm.dmPelsHeight;
		it = std::find_if(pResolutions->begin(), pResolutions->end(), stFindResolution(Resolution));
		if(dm.dmBitsPerPel >= MinBitsPerPixel  && dm.dmPelsWidth >= MinResolutionX && it == pResolutions->end())
		{
			pResolutions->push_back(Resolution);
			k ++;
		}
	}
	std::sort(pResolutions->begin(), pResolutions->end(), ResolutionSort);
	
	if(!pResolutions->size()) return false;
	
	return true;
}
