#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imAcq.h"

using namespace tld;

ImAcq * imAcqAlloc() {
	ImAcq * imAcq = (ImAcq *)malloc(sizeof(ImAcq));
	imAcq->method = IMACQ_CAM;
	imAcq->currentFrame = 1;
	imAcq->lastFrame = 0;
	return imAcq;
}

void imAcqInit(ImAcq * imAcq) {
	if(imAcq->method == IMACQ_CAM) {
		imAcq->capture = cvCaptureFromCAM(0);
		if(imAcq->capture == NULL) {
			printf("Error: Unable to initialize camera\n");
			exit(0);
		}
	} else if(imAcq->method == IMACQ_VID) {
		imAcq->capture = cvCaptureFromAVI(imAcq->imgPath);
		if(imAcq->capture == NULL) {
			printf("Error: Unable to open video\n");
			exit(0);
		}
		// take all frames
		if(imAcq->lastFrame == 0)
			imAcq->lastFrame = imAcqVidGetNumberOfFrames(imAcq);
		// lastFrame out of bounds
		if(imAcq->lastFrame > imAcqVidGetNumberOfFrames(imAcq)) {
			printf("Error: video has only %d frames you selected %d as last frame.\n",
					imAcqVidGetNumberOfFrames(imAcq), imAcq->lastFrame);
			exit(1);
		}
		// something is wrong with startFrame and/or lastFrame
		if((imAcq->lastFrame < 1) || (imAcq->currentFrame < 1) || ((imAcq->currentFrame > imAcq->lastFrame))) {
			printf("Error: something is wrong with the start and last frame number. startFrame: %d lastFrame: %d\n",
					imAcq->currentFrame, imAcq->lastFrame);
			exit(1);
		}
		// set the video position to the correct frame
		imAcqVidSetNextFrameNumber(imAcq, imAcq->currentFrame);
	}
}

void imAcqFree(ImAcq * imAcq) {
	if((imAcq->method == IMACQ_CAM) || (imAcq->method == IMACQ_VID)) {
		cvReleaseCapture(&imAcq->capture);
	}

	free(imAcq);
}

IplImage * imAcqLoadImg(ImAcq * imAcq, char * path) {
	IplImage * image = cvLoadImage(path);

	if(image == NULL) {
		printf("Error: %s does not exist or is not an image.\n", path);
	}

	return image;

}

IplImage * imAcqLoadCurrentFrame(ImAcq * imAcq) {

    char currentPath[255];

	sprintf(currentPath, imAcq->imgPath, imAcq->currentFrame);

	return imAcqLoadImg(imAcq, currentPath);
}

IplImage * imAcqGetImg(ImAcq * imAcq) {

	IplImage * img = NULL;

	if(imAcq->method == IMACQ_CAM || imAcq->method == IMACQ_VID) {
		img = imAcqGrab(imAcq->capture);
	}

	if(imAcq->method == IMACQ_IMGS) {
		img = imAcqLoadCurrentFrame(imAcq);
	}

	imAcqAdvance(imAcq);

	return img;
}

IplImage * imAcqGrab(CvCapture * capture)
{
	IplImage* frame;

	frame = cvQueryFrame( capture );

	if(frame == NULL) {
		printf("Error: Unable to grab image from video\n");
		return NULL;
	}

	return cvCloneImage(frame);
}

IplImage * imAcqGetImgByFrame(ImAcq * imAcq, int fNo) {
	int oldFNo = imAcq->currentFrame;
	imAcq->currentFrame = fNo;

	IplImage * img = imAcqGetImg(imAcq);

	imAcq->currentFrame = oldFNo;

	return img;
}

IplImage * imAcqGetImgAndAdvance(ImAcq * imAcq) {
	IplImage * img = imAcqGetImg(imAcq);
	imAcq->currentFrame++;

	return img;
}

void imAcqAdvance(ImAcq * imAcq) {
	imAcq->currentFrame++;
}

int imAcqHasMoreFrames(ImAcq * imAcq) {
	if(imAcq->lastFrame < 1) return 1;

	if(imAcq->currentFrame > imAcq->lastFrame) {
		return 0;
	} else {
		return 1;
	}
}

int imAcqVidGetNextFrameNumber(ImAcq * imAcq) {
	// OpenCV index starts with 0
	// maybe a OpenCV bug: cvGetCaptureProperty with CV_CAP_PROP_POS_FRAMES returns the LAST
	// frame number to be encoded not the NEXT
	return ((int) cvGetCaptureProperty(imAcq->capture , CV_CAP_PROP_POS_FRAMES))+2;
}

void imAcqVidSetNextFrameNumber(ImAcq * imAcq, int nFrame) {
	// OpenCV index starts with 0
	cvSetCaptureProperty(imAcq->capture , CV_CAP_PROP_POS_FRAMES, nFrame-2.0);
}

int imAcqVidGetNumberOfFrames(ImAcq * imAcq) {
	return ((int) cvGetCaptureProperty(imAcq->capture , CV_CAP_PROP_FRAME_COUNT));
}
