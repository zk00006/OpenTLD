/*
 * ForegroundDetector.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef FOREGROUNDDETECTOR_H_
#define FOREGROUNDDETECTOR_H_

#include <vector>
#include <cv.h>

#include "DetectionResult.h"

using namespace std;
using namespace cv;

namespace tld {

class ForegroundDetector {
public:
	int fgThreshold;
	int minBlobSize;
	Mat bgImg;
	DetectionResult * detectionResult;

	ForegroundDetector();
	virtual ~ForegroundDetector();
	void release();
	void nextIteration(Mat img);
	bool isActive();
};

} /* namespace tld */
#endif /* FOREGROUNDDETECTOR_H_ */
