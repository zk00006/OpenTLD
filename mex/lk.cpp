// Copyright 2011 Zdenek Kalal
//
// This file is part of TLD.
// 
// TLD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// TLD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TLD.  If not, see <http://www.gnu.org/licenses/>.

#include "cv.h"
#include "highgui.h"
#include "math.h"
#include <limits>
#ifdef _CHAR16T
#define CHAR16_T
#endif
#include "mex.h" 


const int MAX_COUNT = 500;
const int MAX_IMG   = 2;
int win_size = 4;
CvPoint2D32f* points[3] = {0,0,0};
static IplImage **IMG = 0;
static IplImage **PYR = 0;

void loadImageFromMatlab(const mxArray *mxImage, IplImage *image) {

	unsigned char *values =  (unsigned char *) mxGetPr(mxImage);
	int widthStep = image->widthStep;
	int N = mxGetN(mxImage); // width
	int M = mxGetM(mxImage); // height

	if (N == 0 || M == 0) {
		printf("Input image error\n");
		return;
	}

	for(int i=0;i<N;i++)
		for(int j=0;j<M;j++) 
			image->imageData[j*widthStep+i] = values[j+i*M];
}

void euclideanDistance (CvPoint2D32f *point1, CvPoint2D32f *point2, float *match, int nPts) {

	for (int i = 0; i < nPts; i++) {

		match[i] = sqrt((point1[i].x - point2[i].x)*(point1[i].x - point2[i].x) + 
		(point1[i].y - point2[i].y)*(point1[i].y - point2[i].y) );

	}
}

void normCrossCorrelation(IplImage *imgI, IplImage *imgJ, CvPoint2D32f *points0, CvPoint2D32f *points1, int nPts, char *status, float *match,int winsize, int method) {


	IplImage *rec0 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
	IplImage *rec1 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
	IplImage *res  = cvCreateImage( cvSize( 1, 1 ), IPL_DEPTH_32F, 1 );

	for (int i = 0; i < nPts; i++) {
		if (status[i] == 1) {
			cvGetRectSubPix( imgI, rec0, points0[i] );
			cvGetRectSubPix( imgJ, rec1, points1[i] );
			cvMatchTemplate( rec0,rec1, res, method );
			match[i] = ((float *)(res->imageData))[0]; 

		} else {
			match[i] = 0.0;
		}
	}
	cvReleaseImage( &rec0 );
	cvReleaseImage( &rec1 );
	cvReleaseImage( &res );

}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

	double nan = std::numeric_limits<double>::quiet_NaN();
	double inf = std::numeric_limits<double>::infinity();

	if (nrhs == 0) {
		mexPrintf("Lucas-Kanade\n");
		return;
	}


	switch ((int) *mxGetPr(prhs[0])) {

		// initialize or clean up
		case 0: {
	
			if (IMG!=0 && PYR!=0) {

				for (int i = 0; i < MAX_IMG; i++) {
					cvReleaseImage(&(IMG[i])); IMG[i] = 0;
					cvReleaseImage(&(PYR[i])); PYR[i] = 0;
				}
				free(IMG); IMG = 0;
				free(PYR); PYR = 0;
				//mexPrintf("LK: deallocated\n");
			}

			IMG = (IplImage**) calloc(MAX_IMG,sizeof(IplImage*));
			PYR = (IplImage**) calloc(MAX_IMG,sizeof(IplImage*));
			//mexPrintf("LK: initialized\n");

			return;
				}

		// tracking
		case 2: {

			if (IMG == 0 || (nrhs != 5 && nrhs != 6)) {
				mexPrintf("lk(2,imgI,imgJ,ptsI,ptsJ,Level)\n");
				//            0 1    2    3    4   
				return;
			}
			int Level;
			if (nrhs == 6) {
				Level = (int) *mxGetPr(prhs[5]);
			} else {
				Level = 5;
			}


			int I       = 0;
			int J       = 1;
			int Winsize = 10;

			// Images
			if (IMG[I] != 0) {
				loadImageFromMatlab(prhs[1],IMG[I]);
			} else {
				CvSize imageSize = cvSize(mxGetN(prhs[1]),mxGetM(prhs[1]));
				IMG[I] = cvCreateImage( imageSize, 8, 1 );
				PYR[I] = cvCreateImage( imageSize, 8, 1 );
				loadImageFromMatlab(prhs[1],IMG[I]);
			}

			if (IMG[J] != 0) {
				loadImageFromMatlab(prhs[2],IMG[J]);
			} else {
				CvSize imageSize = cvSize(mxGetN(prhs[2]),mxGetM(prhs[2]));
				IMG[J] = cvCreateImage( imageSize, 8, 1 );
				PYR[J] = cvCreateImage( imageSize, 8, 1 );
				loadImageFromMatlab(prhs[2],IMG[J]);
			}

			// Points
			double *ptsI = mxGetPr(prhs[3]); int nPts = mxGetN(prhs[3]);
			double *ptsJ = mxGetPr(prhs[4]); 

			if (nPts != mxGetN(prhs[4])) {
				mexPrintf("Inconsistent input!\n");
				return;
			}

			points[0] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // template
			points[1] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // target
			points[2] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // forward-backward

			for (int i = 0; i < nPts; i++) {
				points[0][i].x = ptsI[2*i]; points[0][i].y = ptsI[2*i+1];
				points[1][i].x = ptsJ[2*i]; points[1][i].y = ptsJ[2*i+1];
				points[2][i].x = ptsI[2*i]; points[2][i].y = ptsI[2*i+1];
			}

			float *ncc    = (float*) cvAlloc(nPts*sizeof(float));
			float *ssd    = (float*) cvAlloc(nPts*sizeof(float));
			float *fb     = (float*) cvAlloc(nPts*sizeof(float));
			char  *status = (char*)  cvAlloc(nPts);

			cvCalcOpticalFlowPyrLK( IMG[I], IMG[J], PYR[I], PYR[J], points[0], points[1], nPts, cvSize(win_size,win_size), Level, status, 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), CV_LKFLOW_INITIAL_GUESSES);
			cvCalcOpticalFlowPyrLK( IMG[J], IMG[I], PYR[J], PYR[I], points[1], points[2], nPts, cvSize(win_size,win_size), Level, 0     , 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY );
			
			normCrossCorrelation(IMG[I],IMG[J],points[0],points[1],nPts, status, ncc, Winsize,CV_TM_CCOEFF_NORMED);
			//normCrossCorrelation(IMG[I],IMG[J],points[0],points[1],nPts, status, ssd, Winsize,CV_TM_SQDIFF);
			euclideanDistance( points[0],points[2],fb,nPts);
			

			// Output
			int M = 4;
			plhs[0] = mxCreateDoubleMatrix(M, nPts, mxREAL);
			double *output = mxGetPr(plhs[0]);
			for (int i = 0; i < nPts; i++) {
				if (status[i] == 1) {
					output[M*i]   = (double) points[1][i].x;
					output[M*i+1] = (double) points[1][i].y;
					output[M*i+2] = (double) fb[i];
					output[M*i+3] = (double) ncc[i];
					//output[M*i+4] = (double) ssd[i];
				} else {
					output[M*i]   = nan;
					output[M*i+1] = nan;
					output[M*i+2] = nan;
					output[M*i+3] = nan;
					//output[M*i+4] = nan;
				}
			}

			return;
				}

	}

}


