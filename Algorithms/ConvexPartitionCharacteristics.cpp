#include "pch.h"
#include "ConvexPartitionCharacteristics.h"

namespace algo {

	ConvexPartitionCharacteristics::ConvexPartitionCharacteristics() {}

	ConvexPartitionCharacteristics::ConvexPartitionCharacteristics(double areaOfNotSplittedParts, double percentageOfNotSplittedParts, double optimizationFuncValue) {
		this->areaOfNotSplittedParts = areaOfNotSplittedParts;
		this->percentageOfNotSplittedParts = percentageOfNotSplittedParts;
		this->optimizationFuncValue = optimizationFuncValue;
	}

	double ConvexPartitionCharacteristics::getAreaOfNotSplittedParts(int prec) {
		return getDoubleWithPrecision(areaOfNotSplittedParts, prec);
	}

	double ConvexPartitionCharacteristics::getPercentageOfNotSplittedParts(int prec) {
		return getDoubleWithPrecision(percentageOfNotSplittedParts, prec);
	}

	double ConvexPartitionCharacteristics::getOptimizationFuncValue(int prec) {
		return getDoubleWithPrecision(optimizationFuncValue, prec);
	}

	double ConvexPartitionCharacteristics::getDoubleWithPrecision(double number, int prec) {
		int count = prec;

		while (count > 0) {
			number *= 10;
			count--;
		}

		number = (int)number;

		count = prec;

		while (count > 0) {
			number /= 10;
			count--;
		}

		return number;
	}

}