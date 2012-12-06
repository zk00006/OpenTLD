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
 * DetectionResult.h
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#ifndef DETECTIONRESULT_H_
#define DETECTIONRESULT_H_

#include <vector>

#include <opencv/cv.h>

namespace tld
{

class DetectionResult
{
public:
    bool containsValidData;
    std::vector<cv::Rect>* fgList;
    float *posteriors;  /* Contains the posteriors for each slding window. Is of size numWindows. Allocated by tldInitClassifier. */
    std::vector<int>* confidentIndices;
    int *featureVectors;
    float *variances;
    int numClusters;
    cv::Rect *detectorBB; //Contains a valid result only if numClusters = 1

    DetectionResult();
    virtual ~DetectionResult();

    void init(int numWindows, int numTrees);

    void reset();
    void release();

};

} /* namespace tld */
#endif /* DETECTIONRESULT_H_ */
