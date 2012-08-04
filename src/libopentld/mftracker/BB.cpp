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
 * @file bb.c
 *
 *  Created on: 17.03.2011
 *     @author: bernd
 * @brief
 */

#include "BB.h"

int calculateBBCenter(float bb[4], float center[2]);

/**
 * Creates numM x numN points grid on BBox.
 * Points ordered in 1 dimensional array (x1, y1, x2, y2).
 * @param bb        Bounding box represented through 2 points(x1,y1,x2,y2)
 * @param numM      Number of points in height direction.
 * @param numN      Number of points in width direction.
 * @param margin    margin (in pixel)
 * @param pts       Contains the calculated points in the form (x1, y1, x2, y2).
 *                  Size of the array must be numM * numN * 2.
 */
int getFilledBBPoints(float *bb, int numM, int numN, int margin, float *pts)
{
    int pointDim = 2;
    int i;
    int j;
    /**
     * gap between points in width direction
     */
    float divN;
    /**
     * gap between points in height direction
     */
    float divM;
    float bb_local[4];
    float center[2];
    float spaceN;
    float spaceM;
    /*add margin*/
    bb_local[0] = bb[0] + margin;
    bb_local[1] = bb[1] + margin;
    bb_local[2] = bb[2] - margin;
    bb_local[3] = bb[3] - margin;

    /*  printf("PointArraySize should be: %d\n", numM * numN * pointDim);*/
    /*handle cases numX = 1*/
    if(numN == 1 && numM == 1)
    {
        calculateBBCenter(bb_local, pts);
        return 1;
    }
    else if(numN == 1 && numM > 1)
    {
        divM = numM - 1;
        divN = 2;
        /*maybe save center coordinate into bb[1] instead of loop again*/
        /*calculate step width*/
        spaceM = (bb_local[3] - bb_local[1]) / divM;
        calculateBBCenter(bb_local, center);

        /*calculate points and save them to the array*/
        for(i = 0; i < numN; i++)
        {
            for(j = 0; j < numM; j++)
            {
                pts[i * numM * pointDim + j * pointDim + 0] = center[0];
                pts[i * numM * pointDim + j * pointDim + 1] = bb_local[1] + j * spaceM;
            }
        }

        return 1;
    }
    else if(numN > 1 && numM == 1)
    {
        float center[2];
        float *cen;
        divM = 2;
        divN = numN - 1;
        //maybe save center coordinate into bb[1] instead of loop again
        //calculate step width
        spaceN = (bb_local[2] - bb_local[0]) / divN;
        cen = center;
        calculateBBCenter(bb_local, center);

        //calculate points and save them to the array
        for(i = 0; i < numN; i++)
        {
            for(j = 0; j < numM; j++)
            {
                pts[i * numM * pointDim + j * pointDim + 0] = bb_local[0] + i * spaceN;
                pts[i * numM * pointDim + j * pointDim + 1] = cen[1];
            }
        }

        return 1;
    }
    else if(numN > 1 && numM > 1)
    {
        divM = numM - 1;
        divN = numN - 1;
    }

    //calculate step width
    spaceN = (bb_local[2] - bb_local[0]) / divN;
    spaceM = (bb_local[3] - bb_local[1]) / divM;

    //calculate points and save them to the array
    for(i = 0; i < numN; i++)
    {
        for(j = 0; j < numM; j++)
        {
            pts[i * numM * pointDim + j * pointDim + 0] = bb_local[0] + i * spaceN;
            pts[i * numM * pointDim + j * pointDim + 1] = bb_local[1] + j * spaceM;
        }
    }

    return 1;
}

/**
 * Calculates center of a Rectangle/Boundingbox.
 * @param bb defined with 2 points x,y,x1,y1
 * @param center Output. point center[0]=x,center[1]=y
 * @return 0 if bb is NULL and the center couldn't be computed, 1 on success.
 */
int calculateBBCenter(float bb[4], float center[2])
{
    if(bb == 0)
        return 0;

    center[0] = 0.5 * (bb[0] + bb[2]);
    center[1] = 0.5 * (bb[1] + bb[3]);
    return 1;
}
