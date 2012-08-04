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
 * imAcq.h
 *
 *  Created on: 18 May 2011
 *      Author: Georg Nebehay
 */

#ifndef IMACQ_IMPL_H_
#define IMACQ_IMPL_H_

#include <opencv/highgui.h>

/**
 * Capturing method
 */
enum ImacqMethod
{
    IMACQ_IMGS, //!< Images
    IMACQ_CAM, //!< Camera
    IMACQ_VID, //!< Video
    IMACQ_LIVESIM //!< Livesim
};

typedef struct
{
    int method;
    const char *imgPath;
    CvCapture *capture;
    int lastFrame;
    int currentFrame;
    int startFrame;
    int camNo;
    double startTime;
    float fps;
} ImAcq ;

ImAcq *imAcqAlloc();

void imAcqInit(ImAcq *imAcq);

void imAcqRelease(ImAcq *imAcq);

void imAcqVidSetNextFrameNumber(ImAcq *imAcq, int nFrame);
int imAcqVidGetNextFrameNumber(ImAcq *imAcq);
int imAcqVidGetNumberOfFrames(ImAcq *imAcq);
int imAcqHasMoreFrames(ImAcq *imAcq);
IplImage *imAcqGetImgAndAdvance(ImAcq *imAcq);
IplImage *imAcqGetImg(ImAcq *imAcq);
IplImage *imAcqGetImgByFrame(ImAcq *imAcq, int fNo);
IplImage *imAcqGetImgByCurrentTime(ImAcq *imAcq);
IplImage *imAcqLoadImg(ImAcq *imAcq, char *path);
IplImage *imAcqLoadCurrentFrame(ImAcq *imAcq);
IplImage *imAcqLoadVidFrame(CvCapture *capture);
IplImage *imAcqGrab(CvCapture *capture);
void imAcqAdvance(ImAcq *imAcq);
void imAcqFree(ImAcq *);

#endif /* IMACQ_H_ */
