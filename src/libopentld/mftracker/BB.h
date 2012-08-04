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
 * @file bb.h
 *
 *  Created on: 17.03.2011
 *     @author: bernd
 * @brief
 */

#ifndef BB_H_
#define BB_H_

/**
 * @param bb        Bounding box represented through 2 points(x1,y1,x2,y2)
 * @param numM      Number of points in height direction.
 * @param numN      Number of points in width direction.
 * @param margin    margin (in pixel)
 * @param pts       Contains the calculated points in the form (x1, y1, x2, y2).
 *                  Size of the array must be numM * numN * 2.
 */
int getFilledBBPoints(float *bb, int numM, int numN, int margin, float *pts);

#endif /* BB_H_ */
