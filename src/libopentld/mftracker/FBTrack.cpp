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

/**
 * @file track.c
 *
 *  Created on: 01.03.2011
 *      @author bernd
 */

#include "FBTrack.h"

#include <cstdio>

#include "BB.h"
#include "BBPredict.h"
#include "Median.h"
#include "Lk.h"

/**
 * Calculate the bounding box of an Object in a following Image.
 * Imgs aren't changed.
 * @param imgI       Image contain Object with known BoundingBox
 * @param imgJ       Following Image.
 * @param bb         Bounding box of object to track in imgI.
 *                   Format x1,y1,x2,y2
 * @param scaleshift returns relative scale change of bb
 */
int fbtrack(IplImage *imgI, IplImage *imgJ, float *bb, float *bbnew,
            float *scaleshift)
{
    char level = 5;
    const int numM = 10;
    const int numN = 10;
    const int nPoints = numM * numN;
    const int sizePointsArray = nPoints * 2;

    float fb[nPoints];
    float ncc[nPoints];
    char status[nPoints];

    float pt[sizePointsArray];
    float ptTracked[sizePointsArray];
    int nlkPoints;
    CvPoint2D32f *startPoints;
    CvPoint2D32f *targetPoints;
    float *fbLkCleaned;
    float *nccLkCleaned;
    int i, M;
    int nRealPoints;
    float medFb;
    float medNcc;
    int nAfterFbUsage;
    getFilledBBPoints(bb, numM, numN, 5, pt);
    //getFilledBBPoints(bb, numM, numN, 5, &ptTracked);
    memcpy(ptTracked, pt, sizeof(float) * sizePointsArray);

    initImgs();
    trackLK(imgI, imgJ, pt, nPoints, ptTracked, nPoints, level, fb, ncc, status);
    initImgs();
    //  char* status = *statusP;
    nlkPoints = 0;

    for(i = 0; i < nPoints; i++)
    {
        nlkPoints += status[i];
    }

    startPoints = (CvPoint2D32f *) malloc(nlkPoints * sizeof(CvPoint2D32f));
    targetPoints = (CvPoint2D32f *) malloc(nlkPoints * sizeof(CvPoint2D32f));
    fbLkCleaned = (float *) malloc(nlkPoints * sizeof(float));
    nccLkCleaned = (float *) malloc(nlkPoints * sizeof(float));

    M = 2;
    nRealPoints = 0;

    for(i = 0; i < nPoints; i++)
    {
        //TODO:handle Missing Points
        //or status[i]==0
        if(ptTracked[M * i] == -1)
        {
        }
        else
        {
            startPoints[nRealPoints].x = pt[2 * i];
            startPoints[nRealPoints].y = pt[2 * i + 1];
            targetPoints[nRealPoints].x = ptTracked[2 * i];
            targetPoints[nRealPoints].y = ptTracked[2 * i + 1];
            fbLkCleaned[nRealPoints] = fb[i];
            nccLkCleaned[nRealPoints] = ncc[i];
            nRealPoints++;
        }
    }

    //assert nRealPoints==nlkPoints
    medFb = getMedian(fbLkCleaned, nlkPoints);
    medNcc = getMedian(nccLkCleaned, nlkPoints);
    /*  printf("medianfb: %f\nmedianncc: %f\n", medFb, medNcc);
     printf("Number of points after lk: %d\n", nlkPoints);*/
    nAfterFbUsage = 0;

    for(i = 0; i < nlkPoints; i++)
    {
        if((fbLkCleaned[i] <= medFb) & (nccLkCleaned[i] >= medNcc))
        {
            startPoints[nAfterFbUsage] = startPoints[i];
            targetPoints[nAfterFbUsage] = targetPoints[i];
            nAfterFbUsage++;
        }
    }

    /*printf("Number of points after fb correction: %d\n", nAfterFbUsage);*/
    //  showIplImage(IMGS[1]);
    // show "OpticalFlow" fb filtered.
    //  drawLinesCvPoint2D32f(imgI, startPoints, nRealPoints, targetPoints,
    //      nRealPoints);
    //  showIplImage(imgI);

    predictbb(bb, startPoints, targetPoints, nAfterFbUsage, bbnew, scaleshift);
    /*printf("bbnew: %f,%f,%f,%f\n", bbnew[0], bbnew[1], bbnew[2], bbnew[3]);
     printf("relative scale: %f \n", scaleshift[0]);*/
    //show picture with tracked bb
    //  drawRectFromBB(imgJ, bbnew);
    //  showIplImage(imgJ);
    free(startPoints);
    free(targetPoints);
    free(fbLkCleaned);
    free(nccLkCleaned);

    if(medFb > 10) return 0;
    else return 1;

}
