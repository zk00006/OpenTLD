/*
 * DetectionResult.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#include "DetectionResult.h"
#include "TLDUtil.h"

namespace tld {

DetectionResult::DetectionResult() {
	containsValidData = false;
	fgList = new vector<Rect>();
	confidentIndices = new vector<int>();
	numClusters = 0;
	detectorBB = NULL;

	variances = NULL;
	posteriors = NULL;
	featureVectors = NULL;
}

DetectionResult::~DetectionResult() {
	release();
}

void DetectionResult::init(int numWindows, int numTrees) {
	variances = new float[numWindows];
	posteriors = new float[numWindows];
	featureVectors = new int[numWindows*numTrees];
	confidentIndices = new vector<int>();

}

void DetectionResult::reset() {
	containsValidData = false;
	fgList->clear();
	confidentIndices->clear();
	numClusters = 0;
	delete detectorBB;
	detectorBB = NULL;
}

void DetectionResult::release() {
	fgList->clear();
	delete[] variances;
	variances = NULL;
	delete[] posteriors;
	posteriors = NULL;
	delete[] featureVectors;
	featureVectors = NULL;
	delete confidentIndices;
	confidentIndices = NULL;
	delete detectorBB;
	detectorBB = NULL;
}

} /* namespace tld */
