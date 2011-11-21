/*
 * VarianceFilter.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#include "VarianceFilter.h"
#include "IntegralImage.h"
#include "DetectorCascade.h"

namespace tld {

VarianceFilter::VarianceFilter() {
	minVar = 0;
	integralImg = NULL;
	integralImg_squared = NULL;
}

VarianceFilter::~VarianceFilter() {
	release();
}

void VarianceFilter::release() {
	if(integralImg != NULL) delete integralImg;
	integralImg = NULL;

	if(integralImg_squared != NULL) delete integralImg_squared;
	integralImg_squared = NULL;
}

float VarianceFilter::calcVariance(int *off) {

	int * ii1 = integralImg->data;
	long long * ii2 = integralImg_squared->data;

	float mX  = (ii1[off[3]] - ii1[off[2]] - ii1[off[1]] + ii1[off[0]]) / (float) off[5]; //Sum of Area divided by area
	float mX2 = (ii2[off[3]] - ii2[off[2]] - ii2[off[1]] + ii2[off[0]]) / (float) off[5];
	return mX2 - mX*mX;
}

void VarianceFilter::nextIteration(Mat img) {
	release();

	integralImg = new IntegralImage<int>(img.size());
	integralImg->calcIntImg(img);

	integralImg_squared = new IntegralImage<long long>(img.size());
	integralImg_squared->calcIntImg(img, true);
}

bool VarianceFilter::filter(int i) {
	float bboxvar = calcVariance(windowOffsets + TLD_WINDOW_OFFSET_SIZE*i);

	detectionResult->variances[i] = bboxvar;

	if(bboxvar < minVar) {
		return false;
	}

	return true;
}

} /* namespace tld */
