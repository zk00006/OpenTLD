/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/*
 * TLD.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "TLD.h"
#include "NNClassifier.h"
#include "TLDUtil.h"
#include <iostream>

using namespace std;

namespace tld {

TLD::TLD() {
	trackerEnabled = true;
	detectorEnabled = true;
	learningEnabled = true;
	alternating = false;
	valid = false;
	wasValid = false;
	learning = false;
	currBB = NULL;

	detectorCascade = new DetectorCascade();
	nnClassifier = detectorCascade->nnClassifier;

	medianFlowTracker = new MedianFlowTracker();
}

TLD::~TLD() {
	storeCurrentData();

	delete detectorCascade;
	delete medianFlowTracker;
}

void TLD::release() {
	detectorCascade->release();
	medianFlowTracker->cleanPreviousData();
	delete currBB;
}

void TLD::storeCurrentData() {
	prevImg.release();
	prevImg = currImg; //Store old image (if any)
	prevBB = currBB;		//Store old bounding box (if any)

	detectorCascade->cleanPreviousData(); //Reset detector results
	medianFlowTracker->cleanPreviousData();

	wasValid = valid;
}

void TLD::selectObject(Mat img, Rect * bb) {
	//Delete old object
	detectorCascade->release();

	detectorCascade->objWidth = bb->width;
	detectorCascade->objHeight = bb->height;

	//Init detector cascade
	detectorCascade->init();

	currImg = img;
	currBB = bb;
	currConf = 1;
	valid = true;

	initialLearning();

}

//img must be a non-null grayscale uint8 image
void TLD::processImage(Mat img) {
	storeCurrentData();

	currImg = img;	//Store new image

	if(trackerEnabled) {
		medianFlowTracker->track(prevImg, currImg, prevBB);
	}

	if(detectorEnabled && (!alternating || medianFlowTracker->trackerBB == NULL)) {
		detectorCascade->detect(img);
	}

	fuseHypotheses();

	learn();

}

void TLD::fuseHypotheses() {
	Rect* trackerBB = medianFlowTracker->trackerBB;
	int numClusters = detectorCascade->detectionResult->numClusters;
	Rect* detectorBB = detectorCascade->detectionResult->detectorBB;


	currBB = NULL;
	currConf = 0;
	valid = false;

	float confDetector = 0;

	if(numClusters == 1) {
		confDetector = nnClassifier->classifyBB(currImg, detectorBB);
	}

	if(trackerBB != NULL) {
		float confTracker = nnClassifier->classifyBB(currImg, trackerBB);

		if(numClusters == 1 && confDetector > confTracker && tldOverlapRectRect(*trackerBB, *detectorBB) < 0.5) {

			currBB = tldCopyRect(detectorBB);
			currConf = confDetector;
		} else {
			currBB = tldCopyRect(trackerBB);
			currConf = confTracker;
			if(confTracker > nnClassifier->thetaTP) {
				valid = true;
			} else if(wasValid && confTracker > nnClassifier->thetaFP) {
				valid = true;
			}
		}
	} else if(numClusters == 1) {
		currBB = tldCopyRect(detectorBB);
		currConf = confDetector;
	}

	/*
	float var = CalculateVariance(patch.values, nn->patch_size*nn->patch_size);

	if(var < min_var) { //TODO: Think about incorporating this
		printf("%f, %f: Variance too low \n", var, classifier->min_var);
		valid = 0;
	}*/
}

void TLD::initialLearning() {
	learning = true; //This is just for display purposes

	DetectionResult* detectionResult = detectorCascade->detectionResult;

	detectorCascade->detect(currImg);

	//This is the positive patch
	NormalizedPatch patch;
	tldExtractNormalizedPatchRect(currImg, currBB, patch.values);
	patch.positive = 1;

	float initVar = tldCalcVariance(patch.values, TLD_PATCH_SIZE*TLD_PATCH_SIZE);
	detectorCascade->varianceFilter->minVar = initVar/2;


	float * overlap = new float[detectorCascade->numWindows];
	tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB,overlap);

	//Add all bounding boxes with high overlap

	vector< pair<int,float> > positiveIndices;
	vector<int> negativeIndices;

	//First: Find overlapping positive and negative patches

	for(int i = 0; i < detectorCascade->numWindows; i++) {

		if(overlap[i] > 0.6) {
			positiveIndices.push_back(pair<int,float>(i,overlap[i]));
		}

		if(overlap[i] < 0.2) {
			float variance = detectionResult->variances[i];

			if(variance > detectorCascade->varianceFilter->minVar) { //TODO: This check is unnecessary if minVar would be set before calling detect.
				negativeIndices.push_back(i);
			}
		}
	}

	sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

	vector<NormalizedPatch> patches;

	patches.push_back(patch); //Add first patch to patch list

	int numIterations = min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)
	for(int i = 0; i < numIterations; i++) {
		int idx = positiveIndices.at(i).first;
		//Learn this bounding box
		//TODO: Somewhere here image warping might be possible
		detectorCascade->ensembleClassifier->learn(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE*idx], true, &detectionResult->featureVectors[detectorCascade->numTrees*idx]);
	}

	random_shuffle(negativeIndices.begin(), negativeIndices.end());

	//Choose 100 random patches for negative examples
	for(size_t i = 0; i < min<size_t>(100,negativeIndices.size()); i++) {
		int idx = negativeIndices.at(i);

		NormalizedPatch patch;
		tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE*idx], patch.values);
		patch.positive = 0;
		patches.push_back(patch);
	}

	detectorCascade->nnClassifier->learn(patches);

	delete[] overlap;

}

//Do this when current trajectory is valid
void TLD::learn() {
	if(!learningEnabled || !valid || !detectorEnabled) {
		learning = false;
		return;
	}
	learning = true;

	DetectionResult* detectionResult = detectorCascade->detectionResult;

	if(!detectionResult->containsValidData) {
		detectorCascade->detect(currImg);
	}

	//This is the positive patch
	NormalizedPatch patch;
	tldExtractNormalizedPatchRect(currImg, currBB, patch.values);

	float * overlap = new float[detectorCascade->numWindows];
	tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB,overlap);

	//Add all bounding boxes with high overlap

	vector<pair<int,float> > positiveIndices;
	vector<int> negativeIndices;
	vector<int> negativeIndicesForNN;

	//First: Find overlapping positive and negative patches

	for(int i = 0; i < detectorCascade->numWindows; i++) {

		if(overlap[i] > 0.6) {
			positiveIndices.push_back(pair<int,float>(i,overlap[i]));
		}

		if(overlap[i] < 0.2) {
			if(detectionResult->posteriors[i] > 0.1) {
				negativeIndices.push_back(i);
			}

			if(detectionResult->posteriors[i] > 0.5) {
				negativeIndicesForNN.push_back(i);
			}

		}
	}

	sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

	vector<NormalizedPatch> patches;

	patch.positive = 1;
	patches.push_back(patch);
	//TODO: Flip


	int numIterations = min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)

	for(size_t i = 0; i < negativeIndices.size(); i++) {
		int idx = negativeIndices.at(i);
		//TODO: Somewhere here image warping might be possible
		detectorCascade->ensembleClassifier->learn(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE*idx], false, &detectionResult->featureVectors[detectorCascade->numTrees*idx]);
	}

	//TODO: Randomization might be a good idea
	for(int i = 0; i < numIterations; i++) {
		int idx = positiveIndices.at(i).first;
		//TODO: Somewhere here image warping might be possible
		detectorCascade->ensembleClassifier->learn(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE*idx], true, &detectionResult->featureVectors[detectorCascade->numTrees*idx]);
	}

	for(size_t i = 0; i < negativeIndicesForNN.size(); i++) {
		int idx = negativeIndicesForNN.at(i);

		NormalizedPatch patch;
		tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE*idx], patch.values);
		patch.positive = 0;
		patches.push_back(patch);
	}

	detectorCascade->nnClassifier->learn(patches);

	delete[] overlap;
}


} /* namespace tld */
