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
 * VarianceFilter.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#include "VarianceFilter.h"

#include "IntegralImage.h"
#include "DetectorCascade.h"

using namespace cv;

namespace tld
{

VarianceFilter::VarianceFilter()
{
    enabled = true;
    minVar = 0;
    integralImg = NULL;
    integralImg_squared = NULL;
}

VarianceFilter::~VarianceFilter()
{
    release();
}

void VarianceFilter::release()
{
    if(integralImg != NULL) delete integralImg;

    integralImg = NULL;

    if(integralImg_squared != NULL) delete integralImg_squared;

    integralImg_squared = NULL;
}

float VarianceFilter::calcVariance(int *off)
{

    int *ii1 = integralImg->data;
    long long *ii2 = integralImg_squared->data;

    float mX  = (ii1[off[3]] - ii1[off[2]] - ii1[off[1]] + ii1[off[0]]) / (float) off[5]; //Sum of Area divided by area
    float mX2 = (ii2[off[3]] - ii2[off[2]] - ii2[off[1]] + ii2[off[0]]) / (float) off[5];
    return mX2 - mX * mX;
}

void VarianceFilter::nextIteration(const Mat &img)
{
    if(!enabled) return;

    release();

    integralImg = new IntegralImage<int>(img.size());
    integralImg->calcIntImg(img);

    integralImg_squared = new IntegralImage<long long>(img.size());
    integralImg_squared->calcIntImg(img, true);
}

bool VarianceFilter::filter(int i)
{
    if(!enabled) return true;

    float bboxvar = calcVariance(windowOffsets + TLD_WINDOW_OFFSET_SIZE * i);

    detectionResult->variances[i] = bboxvar;

    if(bboxvar < minVar)
    {
        return false;
    }

    return true;
}

} /* namespace tld */
