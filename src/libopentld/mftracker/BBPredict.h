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
 * @file bb_predict.h
 *
 *  Created on: 28.03.2011
 *     @author: bernd
 * @brief
 */

#ifndef BBPREDICT_H_
#define BBPREDICT_H_

#include <opencv/cv.h>

/**
 * @param bb0   The previous BoundingBox.
 * @param pt0   Feature points in the previous BoundingBox.
 * @param pt1   Feature points of the new BoundingBox.
 * @param nPts  Length of pt0/pt1.
 * @param bb1   Output, contains predicted BoundingBox.
 * @param shift Output, contains relative scale change.
 *              1 == no scalechange, experience: if shift == 0
 *              BoundingBox moved completely out of picture
 *              (not validated)
 */
int predictbb(float *bb0, CvPoint2D32f *pt0, CvPoint2D32f *pt1, int nPts,
              float *bb1, float *shift);

#endif /* BBPREDICT_H_ */
