/*
 * TLD.h
 *
 *  Created on: Nov 17, 2011
 *      Author: georg
 */

#ifndef TLD_H_
#define TLD_H_

#include <cv.h>

#include "MedianFlowTracker.h"
#include "DetectorCascade.h"

using namespace cv;

namespace tld {

class TLD {
	void storeCurrentData();
	void fuseHypotheses();
	void learn();
	void initialLearning();
public:
	bool trackerEnabled;
	bool detectorEnabled;
	bool learningEnabled;
	bool alternating;

	MedianFlowTracker* medianFlowTracker;
	DetectorCascade* detectorCascade;
	NNClassifier* nnClassifier;
	bool valid;
	bool wasValid;
	Mat prevImg;
	Mat currImg;
	Rect* prevBB;
	Rect* currBB;
	float currConf;
	bool learning;

	TLD();
	virtual ~TLD();
	void release();
	void selectObject(Mat img, Rect * bb);
	void processImage(Mat img);
};

} /* namespace tld */
#endif /* TLD_H_ */
