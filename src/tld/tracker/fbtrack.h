/*
 * fbtrack.h
 *
 *  Created on: 29.04.2011
 *      Author: georg
 */

#ifndef FBTRACK_H_
#define FBTRACK_H_
/***********************************************************
 * INCLUDES
 ***********************************************************/
#include <opencv/cv.h>

/***********************************************************
 * FUNCTION
 ***********************************************************/
/*
 * @param imgI       Image contain Object with known BoundingBox
 * @param imgJ       Following Image.
 * @param bb         Bounding box of object to track in imgI.
 *                   Format x1,y1,x2,y2
 * @param scaleshift returns relative scale change of bb
 */
int fbtrack(IplImage *imgI, IplImage *imgJ, float* bb, float* bbnew, float* scaleshift);

#endif /* FBTRACK_H_ */
/***********************************************************
 * END OF FILE
 ***********************************************************/
