#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

namespace algo {

	class ALGORITHMS_API ConvexPartitionCharacteristics
	{
	private:
		double areaOfNotSplittedParts;
		double percentageOfNotSplittedParts;
		double optimizationFuncValue;

	public:
		ConvexPartitionCharacteristics();
		ConvexPartitionCharacteristics(double areaOfNotSplittedParts, double percentageOfNotSplittedParts, double optimizationFuncValue);

		double getAreaOfNotSplittedParts(int prec = 2);
		double getPercentageOfNotSplittedParts(int prec = 2);
		double getOptimizationFuncValue(int prec = 2);

	private:
		double getDoubleWithPrecision(double number, int prec);
	};

}

#include "pch.h"