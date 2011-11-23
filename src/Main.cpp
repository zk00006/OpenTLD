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
 * MainX.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "Main.h"
#include "config.h"
#include "imAcq.h"
#include "gui.h"
#include "export.h"
#include "TLDUtil.h"

void Main::doWork() {

	IplImage * prev = NULL;
	if(selectManually) {
		prev = imAcqGetImg(imAcq);

		CvRect box;
		if(getBBFromUser(prev, box, gui) == PROGRAM_EXIT) {
			return;
		}

		if(initialBB == NULL) {
			initialBB = new int[4];
		}
		initialBB[0] = box.x;
		initialBB[1] = box.y;
		initialBB[2] = box.width;
		initialBB[3] = box.height;
	}

	if(prev == NULL) {
		prev = imAcqGetImg(imAcq); //This must be the first image in the sequence
	}

	//TODO: Remove this line
	prev = imAcqGetImg(imAcq);

	IplImage * prevGrey = cvCreateImage( cvGetSize(prev), 8, 1 );
	cvCvtColor( prev,prevGrey, CV_BGR2GRAY );

	tld->detectorCascade->imgWidth = prevGrey->width;
	tld->detectorCascade->imgHeight = prevGrey->height;
	tld->detectorCascade->imgWidthStep = prevGrey->widthStep;


	FILE * resultsFile = NULL;

	if(printResults != NULL) {
		resultsFile = fopen(printResults, "w");
	}

	if(loadModel && modelPath != NULL) {
		tldReadFromFile(tld, modelPath);
	} else if(initialBB != NULL) {
		Rect bb = tldArrayToRect(initialBB);

		printf("Starting at %d %d %d %d\n", bb.x, bb.y, bb.width, bb.height);

		tld->selectObject(prevGrey, &bb);
	}

	while(imAcqHasMoreFrames(imAcq)) {
		long int elapsedTime = getCurrentTime();

		IplImage * img = imAcqGetImg(imAcq);
		IplImage * grey = cvCreateImage( cvGetSize(img), 8, 1 );
		cvCvtColor( img, grey, CV_BGR2GRAY );

		tld->processImage(grey);

    	if(printResults != NULL) {
			if(tld->currBB != NULL) {
				fprintf(resultsFile, "%d %.2d %.2d %.2d %.2d %f\n", imAcq->currentFrame, tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height, tld->currConf);
			} else {
				fprintf(resultsFile, "%d NaN NaN NaN NaN NaN\n", imAcq->currentFrame);
			}
    	}

		elapsedTime = getCurrentTime() - elapsedTime;

		float fps = 1000/(float)elapsedTime;

		int confident = (tld->currConf >= threshold) ? 1 : 0;

		if(showOutput || saveOutput) {
			char string[128];

			char learningString[10] = "";
			if(tld->learning) {
				strcpy(learningString, "Learning");
			}

			sprintf(string, "#%d,Posterior %.2f; fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame, tld->currConf, fps, tld->detectorCascade->numWindows, learningString);
			CvScalar yellow = CV_RGB(255,255,0);
			CvScalar blue = CV_RGB(0,0,255);
			CvScalar black = CV_RGB(0,0,0);
			CvScalar white = CV_RGB(255,255,255);

			if(tld->currBB != NULL) {
				CvScalar rectangleColor = (confident) ? blue : yellow;
				cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 8, 8, 0);
			}

			CvFont font;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
			cvRectangle(img, cvPoint(0,0), cvPoint(img->width,50), black, CV_FILLED, 8, 0);
			cvPutText(img, string, cvPoint(25,25), &font, white);

			if(showForeground) {

				for(size_t i = 0; i < tld->detectorCascade->detectionResult->fgList->size(); i++) {
					Rect r = tld->detectorCascade->detectionResult->fgList->at(i);
					cvRectangle(img, r.tl(),r.br(), white, 1);
				}

			}


			if(showOutput) {
				gui->showImage(img);
				char key = gui->getKey();
				if(key == 'q') break;

				if(key == 'b') {

					ForegroundDetector* fg = tld->detectorCascade->foregroundDetector;

					if(fg->bgImg.empty()) {
						IplImage * bgGrey = cvCreateImage( cvGetSize(prev), 8, 1 );
						cvCvtColor(img,bgGrey, CV_BGR2GRAY );
						fg->bgImg = bgGrey;
					} else {
						fg->bgImg.release();
					}
				}

				if(key == 'c') {
					//clear everything
					tld->release();
				}

				if(key == 'l') {
					tld->learningEnabled = !tld->learningEnabled;
					printf("LearningEnabled: %d\n", tld->learningEnabled);
				}

				if(key == 'a') {
					tld->alternating = !tld->alternating;
					printf("alternating: %d\n", tld->alternating);
				}

				if(key == 'e') {
					tldWriteToFile(tld, modelExportFile);
				}

				if(key == 'i') {
					tldReadFromFile(tld, modelPath);
				}

				if(key == 'r') {
					CvRect box;
					if(getBBFromUser(img, box, gui) == PROGRAM_EXIT) {
						break;
					}

					Rect r = Rect(box);

					tld->selectObject(grey, &r);
				}
			}

			if(saveOutput) {
				char fileName[256];
				sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame);

				cvSaveImage(fileName, img);
			}
		}

		cvReleaseImage(&img);

	}
}

int main(int argc, char ** argv) {

	Main* main = new Main();
	Config config;
	ImAcq * imAcq = imAcqAlloc();
	Gui * gui = new Gui();

	main->gui = gui;
	main->imAcq = imAcq;

	if(config.init(argc, argv) == PROGRAM_EXIT) {
		return EXIT_FAILURE;
	}

	config.configure(main);

	imAcqInit(imAcq);

	if(main->showOutput) {
		gui->init();
	}

	main->doWork();

	delete main;

	return EXIT_SUCCESS;
}
