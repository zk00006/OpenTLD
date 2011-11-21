/*
 * DetectionResult.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef DETECTIONRESULT_H_
#define DETECTIONRESULT_H_

#include <vector>
#include <cv.h>

using namespace std;
using namespace cv;

namespace tld {

class DetectionResult {
public:
	bool containsValidData;
	vector<Rect>* fgList;
	float * posteriors; /* Contains the posteriors for each slding window. Is of size numWindows. Allocated by tldInitClassifier. */
	vector<int>* confidentIndices;
	int * featureVectors;
	float * variances;
	int numClusters;
	Rect* detectorBB; //Contains a valid result only if numClusters = 1

	DetectionResult();
	virtual ~DetectionResult();

	void init(int numWindows, int numTrees);

	void reset();
	void release();

};

} /* namespace tld */
#endif /* DETECTIONRESULT_H_ */
