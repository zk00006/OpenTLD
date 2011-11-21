/*
 * NNClassifier.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef NNCLASSIFIER_H_
#define NNCLASSIFIER_H_

#include <vector>
#include <cv.h>

#include "NormalizedPatch.h"
#include "DetectionResult.h"

using namespace std;
using namespace cv;

namespace tld {

class NNClassifier {
	float ncc(float *f1,float *f2);
public:
	int * windows;
	float thetaFP;
	float thetaTP;
	DetectionResult* detectionResult;
	vector<NormalizedPatch>* falsePositives;
	vector<NormalizedPatch>* truePositives;

	NNClassifier();
	virtual ~NNClassifier();

	void release();
	float classifyPatch(NormalizedPatch * patch);
	float classifyBB(Mat img, Rect* bb);
	void classifyWindow(Mat img, int windowIdx);
	void learn(vector<NormalizedPatch> patches);
};

} /* namespace tld */
#endif /* NNCLASSIFIER_H_ */
