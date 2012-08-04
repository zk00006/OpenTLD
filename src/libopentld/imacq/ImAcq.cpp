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

#include "ImAcq.h"

#include <cstdio>

#include <opencv/cv.h>
#include <opencv/highgui.h>

ImAcq *imAcqAlloc()
{
    ImAcq *imAcq = (ImAcq *)malloc(sizeof(ImAcq));
    imAcq->method = IMACQ_CAM;
    imAcq->currentFrame = 1;
    imAcq->lastFrame = 0;
    imAcq->camNo = 0;
    imAcq->fps = 24;
    return imAcq;
}

void imAcqInit(ImAcq *imAcq)
{
    if(imAcq->method == IMACQ_CAM)
    {
        imAcq->capture = cvCaptureFromCAM(imAcq->camNo);

        if(imAcq->capture == NULL)
        {
            printf("Error: Unable to initialize camera\n");
            exit(0);
        }
    }
    else if(imAcq->method == IMACQ_VID)
    {
        imAcq->capture = cvCaptureFromAVI(imAcq->imgPath);

        if(imAcq->capture == NULL)
        {
            printf("Error: Unable to open video\n");
            exit(0);
        }

        // take all frames
        if(imAcq->lastFrame == 0)
            imAcq->lastFrame = imAcqVidGetNumberOfFrames(imAcq); //This sometimes returns garbage

        // lastFrame out of bounds
        if(imAcq->lastFrame > imAcqVidGetNumberOfFrames(imAcq))
        {
            printf("Error: video has only %d frames you selected %d as last frame.\n",
                   imAcqVidGetNumberOfFrames(imAcq), imAcq->lastFrame);
            exit(1);
        }

        // something is wrong with startFrame and/or lastFrame
        if((imAcq->lastFrame < 1) || (imAcq->currentFrame < 1) || ((imAcq->currentFrame > imAcq->lastFrame)))
        {
            printf("Error: something is wrong with the start and last frame number. startFrame: %d lastFrame: %d\n",
                   imAcq->currentFrame, imAcq->lastFrame);
            exit(1);
        }

        // set the video position to the correct frame
        //This produces strange results on some videos and is deactivated for now.
        //imAcqVidSetNextFrameNumber(imAcq, imAcq->currentFrame);
    }

    imAcq->startFrame = imAcq->currentFrame;
    imAcq->startTime = cvGetTickCount();
}

void imAcqFree(ImAcq *imAcq)
{
    if((imAcq->method == IMACQ_CAM) || (imAcq->method == IMACQ_VID))
    {
        cvReleaseCapture(&imAcq->capture);
    }

    free(imAcq);
}

IplImage *imAcqLoadImg(ImAcq *imAcq, char *path)
{
    IplImage *image = cvLoadImage(path);

    if(image == NULL)
    {
        printf("Error: %s does not exist or is not an image.\n", path);
    }

    return image;

}

IplImage *imAcqLoadFrame(ImAcq *imAcq, int fNo)
{
    char path[255];
    sprintf(path, imAcq->imgPath, fNo);

    printf("load path %s\n", path);

    return cvLoadImage(path);
}

IplImage *imAcqLoadCurrentFrame(ImAcq *imAcq)
{

    return imAcqLoadFrame(imAcq, imAcq->currentFrame);
}

IplImage *imAcqGetImgByCurrentTime(ImAcq *imAcq)
{
    //Calculate current image number
    if(imAcq->method == IMACQ_CAM)
    {
        //printf("grabbing image from sensor");
        return imAcqGrab(imAcq->capture);
    }

    float secondsPassed = (cvGetTickCount() - imAcq->startTime) / cvGetTickFrequency();
    secondsPassed = secondsPassed / 1000000;

    int framesPassed = secondsPassed * imAcq->fps;

    int currentFrame = imAcq->startFrame + framesPassed;

    if(imAcq->lastFrame > 0 && currentFrame > imAcq->lastFrame) return NULL;

    return imAcqLoadFrame(imAcq, currentFrame);
}

IplImage *imAcqGetImg(ImAcq *imAcq)
{

    IplImage *img = NULL;

    if(imAcq->method == IMACQ_CAM || imAcq->method == IMACQ_VID)
    {
        img = imAcqGrab(imAcq->capture);
    }

    if(imAcq->method == IMACQ_IMGS)
    {
        img = imAcqLoadCurrentFrame(imAcq);
    }

    if(imAcq->method == IMACQ_LIVESIM)
    {
        img = imAcqGetImgByCurrentTime(imAcq);
    }

    imAcqAdvance(imAcq);

    return img;
}

IplImage *imAcqGrab(CvCapture *capture)
{
    IplImage *frame;

    frame = cvQueryFrame(capture);

    if(frame == NULL)
    {
        printf("Error: Unable to grab image from video\n");
        return NULL;
    }

    return cvCloneImage(frame);
}

IplImage *imAcqGetImgByFrame(ImAcq *imAcq, int fNo)
{
    int oldFNo = imAcq->currentFrame;
    imAcq->currentFrame = fNo;

    IplImage *img = imAcqGetImg(imAcq);

    imAcq->currentFrame = oldFNo;

    return img;
}

IplImage *imAcqGetImgAndAdvance(ImAcq *imAcq)
{
    IplImage *img = imAcqGetImg(imAcq);
    imAcq->currentFrame++;

    return img;
}

void imAcqAdvance(ImAcq *imAcq)
{
    imAcq->currentFrame++;
}

int imAcqHasMoreFrames(ImAcq *imAcq)
{
    if(imAcq->lastFrame < 1) return 1;

    if(imAcq->currentFrame > imAcq->lastFrame)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int imAcqVidGetNextFrameNumber(ImAcq *imAcq)
{
    // OpenCV index starts with 0
    // maybe a OpenCV bug: cvGetCaptureProperty with CV_CAP_PROP_POS_FRAMES returns the LAST
    // frame number to be encoded not the NEXT
    return ((int) cvGetCaptureProperty(imAcq->capture , CV_CAP_PROP_POS_FRAMES)) + 2;
}

void imAcqVidSetNextFrameNumber(ImAcq *imAcq, int nFrame)
{
    // OpenCV index starts with 0
    cvSetCaptureProperty(imAcq->capture , CV_CAP_PROP_POS_FRAMES, nFrame - 2.0);
}

int imAcqVidGetNumberOfFrames(ImAcq *imAcq)
{
    return ((int) cvGetCaptureProperty(imAcq->capture , CV_CAP_PROP_FRAME_COUNT));
}
