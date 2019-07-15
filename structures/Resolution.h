#ifndef resolutionH
#define resolutionH

#include <windows.h>
#include <functional>
#include <algorithm>
#include <vector>

struct stResolution
{
	unsigned int X;
	unsigned int Y;
};

struct stFindResolution: std::unary_function<stResolution, bool>
{
	stResolution Resolution;
	stFindResolution(stResolution inResolution)
	{
		Resolution = inResolution;
	}
	bool operator()(stResolution const& R) const
	{
		return (R.X == Resolution.X && R.Y == Resolution.Y);
	}
};

bool ResolutionSort(stResolution A, stResolution B);
stResolution GetCurrentResolution();
bool GetSupportedResolutions(std::vector<stResolution>* pResolutions);

#endif
