/*
 * MedianFlowTracker.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: georg
 */

#include "MedianFlowTracker.h"
#include "fbtrack.h"
#include <cmath>

namespace tld {

MedianFlowTracker::MedianFlowTracker() {
	trackerBB = NULL;
}

MedianFlowTracker::~MedianFlowTracker() {
	cleanPreviousData();
}

void MedianFlowTracker::cleanPreviousData() {
	delete trackerBB;
	trackerBB = NULL;
}

void MedianFlowTracker::track(Mat prevMat, Mat currMat, Rect* prevBB) {
	if(prevBB != NULL) {
		float bb_tracker[] = {prevBB->x, prevBB->y, prevBB->width+prevBB->x-1, prevBB->height+prevBB->y-1};
		float scale;

		IplImage prevImg = prevMat;
		IplImage currImg = currMat;

		int success = fbtrack(&prevImg, &currImg, bb_tracker,bb_tracker,&scale);

		//Extract subimage
		float x,y,w,h;
		x = floor(bb_tracker[0]+0.5);
		y = floor(bb_tracker[1]+0.5);
		w = floor(bb_tracker[2]-bb_tracker[0]+1+0.5);
		h = floor(bb_tracker[3]-bb_tracker[1]+1+0.5);

		if(!success || x < 0 || y < 0 || w <= 0 || h <= 0 || x +w > currMat.cols || y+h > currMat.rows) {
			//Leave it empty
		} else {
			trackerBB = new Rect(x,y,w,h);
		}
	}
}

} /* namespace tld */


