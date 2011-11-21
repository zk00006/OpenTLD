/*
 * VarianceFilter.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef VARIANCEFILTER_H_
#define VARIANCEFILTER_H_

#include <cv.h>
#include "IntegralImage.h"
#include "DetectionResult.h"

using namespace cv;

namespace tld {

class VarianceFilter {
	IntegralImage<int>* integralImg;
	IntegralImage<long long>* integralImg_squared;

public:
	int * windowOffsets;

	DetectionResult * detectionResult;

	float minVar;

	VarianceFilter();
	virtual ~VarianceFilter();

	void release();
	void nextIteration(Mat img);
	bool filter(int idx);
	float calcVariance(int *off);
};

} /* namespace tld */
#endif /* VARIANCEFILTER_H_ */
