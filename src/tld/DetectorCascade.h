/*
 * DetectorCascade.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef DETECTORCASCADE_H_
#define DETECTORCASCADE_H_

#include "DetectionResult.h"
#include "ForegroundDetector.h"
#include "VarianceFilter.h"
#include "EnsembleClassifier.h"
#include "Clustering.h"
#include "NNClassifier.h"



namespace tld {

//Constants
static const int TLD_WINDOW_SIZE = 5;
static const int TLD_WINDOW_OFFSET_SIZE = 6;

class DetectorCascade {
	//Working data
	int numScales;
	Size* scales;
public:
	//Configurable members
	int minScale;
	int maxScale;
	bool useShift;
	float shift;
	int minSize;
	int numFeatures;
	int numTrees;

	//Needed for init
	int imgWidth;
	int imgHeight;
	int imgWidthStep;
	int objWidth;
	int objHeight;

	int numWindows;
	int* windows;
	int* windowOffsets;

	//State data
	bool initialised;

	//Components of Detector Cascade
	ForegroundDetector* foregroundDetector;
	VarianceFilter* varianceFilter;
	EnsembleClassifier* ensembleClassifier;
	Clustering* clustering;
	NNClassifier* nnClassifier;

	DetectionResult* detectionResult;

	void propagateMembers();

	DetectorCascade();
	~DetectorCascade();

	void init();

	void initWindowOffsets();
	void initWindowsAndScales();

	void release();
	void cleanPreviousData();
	void detect(Mat img);
};

} /* namespace tld */
#endif /* DETECTORCASCADE_H_ */
