/*
 * EnsembleClassifier.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef ENSEMBLECLASSIFIER_H_
#define ENSEMBLECLASSIFIER_H_

#include <cv.h>

using namespace cv;

namespace tld {

class EnsembleClassifier {
	unsigned char* img;

	float calcConfidence(int * featureVector);
	int calcFernFeature(int windowIdx, int treeIdx);
	void calcFeatureVector(int windowIdx, int * featureVector);
	void updatePosteriors(int *featureVector, int positive, int amount);
public:
	//Configurable members
	int numTrees;
	int numFeatures;

	int imgWidthStep;
	int numScales;
	Size* scales;

	int* windowOffsets;
	int* featureOffsets;
	float* features;

	int numIndices;

	float * posteriors;
	int * positives;
	int * negatives;

	DetectionResult * detectionResult;

	EnsembleClassifier();
	virtual ~EnsembleClassifier();
	void init();
	void initFeatureLocations();
	void initFeatureOffsets();
	void initPosteriors();
	void release();
	void nextIteration(Mat img);
	void classifyWindow(int windowIdx);
	void updatePosterior(int treeIdx, int idx, int positive, int amount);
	void learn(Mat img, int * boundary, int positive, int * featureVector);
};

} /* namespace tld */
#endif /* ENSEMBLECLASSIFIER_H_ */
