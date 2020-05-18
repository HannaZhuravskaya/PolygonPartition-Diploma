#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

namespace algo {

	struct ALGORITHMS_API Point
	{
		double x;
		double y;
	};

}