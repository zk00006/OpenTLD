/*
 * imAcq.h
 *
 *  Created on: 18 May 2011
 *      Author: georg
 */

#ifndef IMACQ_IMPL_H_
#define IMACQ_IMPL_H_

#include <opencv/highgui.h>

namespace tld {

/**
 * Capturing method
 */
enum ImacqMethod {
	IMACQ_IMGS, //!< Images
	IMACQ_CAM, //!< Camera
	IMACQ_VID //!< Video
};

}

typedef struct {
	int method;
	const char * imgPath;
	CvCapture * capture;
	int lastFrame;
	int currentFrame;
} ImAcq ;

ImAcq * imAcqAlloc();

void imAcqInit(ImAcq * imAcq);

void imAcqRelease(ImAcq * imAcq);

void imAcqVidSetNextFrameNumber(ImAcq * imAcq, int nFrame);
int imAcqVidGetNextFrameNumber(ImAcq * imAcq);
int imAcqVidGetNumberOfFrames(ImAcq * imAcq);
int imAcqHasMoreFrames(ImAcq * imAcq);
IplImage * imAcqGetImgAndAdvance(ImAcq * imAcq);
IplImage * imAcqGetImg(ImAcq * imAcq);
IplImage * imAcqGetImgByFrame(ImAcq * imAcq, int fNo);
IplImage * imAcqLoadImg(ImAcq * imAcq, char * path);
IplImage * imAcqLoadCurrentFrame(ImAcq * imAcq);
IplImage * imAcqLoadVidFrame(CvCapture * capture);
IplImage * imAcqGrab(CvCapture* capture);
void imAcqAdvance(ImAcq * imAcq);
void imAcqFree(ImAcq *);

#endif /* IMACQ_H_ */
