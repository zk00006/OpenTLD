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
 * NNClassifier.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#include "NNClassifier.h"

#include "DetectorCascade.h"
#include "TLDUtil.h"

using namespace std;
using namespace cv;

namespace tld
{

NNClassifier::NNClassifier()
{
    thetaFP = .5;
    thetaTP = .65;

    truePositives = new vector<NormalizedPatch>();
    falsePositives = new vector<NormalizedPatch>();

}

NNClassifier::~NNClassifier()
{
    release();

    delete truePositives;
    delete falsePositives;
}

void NNClassifier::release()
{
    falsePositives->clear();
    truePositives->clear();
}

float NNClassifier::ncc(float *f1, float *f2)
{
    double corr = 0;
    double norm1 = 0;
    double norm2 = 0;

    int size = TLD_PATCH_SIZE * TLD_PATCH_SIZE;

    for(int i = 0; i < size; i++)
    {
        corr += f1[i] * f2[i];
        norm1 += f1[i] * f1[i];
        norm2 += f2[i] * f2[i];
    }

    // normalization to <0,1>

    return (corr / sqrt(norm1 * norm2) + 1) / 2.0;
}

float NNClassifier::classifyPatch(NormalizedPatch *patch)
{

    if(truePositives->empty())
    {
        return 0;
    }

    if(falsePositives->empty())
    {
        return 1;
    }

    float ccorr_max_p = 0;

    //Compare patch to positive patches
    for(size_t i = 0; i < truePositives->size(); i++)
    {
        float ccorr = ncc(truePositives->at(i).values, patch->values);

        if(ccorr > ccorr_max_p)
        {
            ccorr_max_p = ccorr;
        }
    }

    float ccorr_max_n = 0;

    //Compare patch to positive patches
    for(size_t i = 0; i < falsePositives->size(); i++)
    {
        float ccorr = ncc(falsePositives->at(i).values, patch->values);

        if(ccorr > ccorr_max_n)
        {
            ccorr_max_n = ccorr;
        }
    }

    float dN = 1 - ccorr_max_n;
    float dP = 1 - ccorr_max_p;

    float distance = dN / (dN + dP);
    return distance;
}

float NNClassifier::classifyBB(const Mat &img, Rect *bb)
{
    NormalizedPatch patch;

    tldExtractNormalizedPatchRect(img, bb, patch.values);
    return classifyPatch(&patch);

}

float NNClassifier::classifyWindow(const Mat &img, int windowIdx)
{
    NormalizedPatch patch;

    int *bbox = &windows[TLD_WINDOW_SIZE * windowIdx];
    tldExtractNormalizedPatchBB(img, bbox, patch.values);

    return classifyPatch(&patch);
}

bool NNClassifier::filter(const Mat &img, int windowIdx)
{
    if(!enabled) return true;

    float conf = classifyWindow(img, windowIdx);

    if(conf < thetaTP)
    {
        return false;
    }

    return true;
}

void NNClassifier::learn(vector<NormalizedPatch> patches)
{
    //TODO: Randomization might be a good idea here
    for(size_t i = 0; i < patches.size(); i++)
    {

        NormalizedPatch patch = patches[i];

        float conf = classifyPatch(&patch);

        if(patch.positive && conf <= thetaTP)
        {
            truePositives->push_back(patch);
        }

        if(!patch.positive && conf >= thetaFP)
        {
            falsePositives->push_back(patch);
        }
    }

}


} /* namespace tld */
