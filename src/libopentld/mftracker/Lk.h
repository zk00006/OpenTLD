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
 * @file lk.h
 *
 *  Created on: 02.03.2011
 *      @author bernd
 */

#ifndef LK_H_
#define LK_H_

#include <opencv/cv.h>

/**
 * Need before start of trackLK and at the end of the program for cleanup.
 */
void initImgs();
int trackLK(IplImage *imgI, IplImage *imgJ, float ptsI[], int nPtsI,
            float ptsJ[], int nPtsJ, int level, float *fbOut, float *nccOut,
            char *statusOut);

#endif /* LK_H_ */
