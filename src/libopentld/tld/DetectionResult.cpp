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
 * DetectionResult.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#include "DetectionResult.h"

#include "TLDUtil.h"

using namespace cv;
using namespace std;

namespace tld
{

DetectionResult::DetectionResult()
{
    containsValidData = false;
    fgList = new vector<Rect>();
    confidentIndices = new vector<int>();
    numClusters = 0;
    detectorBB = NULL;

    variances = NULL;
    posteriors = NULL;
    featureVectors = NULL;
}

DetectionResult::~DetectionResult()
{
    release();
}

void DetectionResult::init(int numWindows, int numTrees)
{
    variances = new float[numWindows];
    posteriors = new float[numWindows];
    featureVectors = new int[numWindows * numTrees];
    confidentIndices = new vector<int>();

}

void DetectionResult::reset()
{
    containsValidData = false;

    if(fgList != NULL) fgList->clear();

    if(confidentIndices != NULL) confidentIndices->clear();

    numClusters = 0;
    delete detectorBB;
    detectorBB = NULL;
}

void DetectionResult::release()
{
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
    containsValidData = false;
}

} /* namespace tld */
