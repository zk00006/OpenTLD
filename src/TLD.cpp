/*
 * TLD.cpp
 *
 *  Created on: Jun 9, 2011
 *      Author: alantrrs
 */

#include <TLD.h>
using namespace cv;

TLD::TLD()
{
  //patchGenerator(0,0,30,true,0.8,1.2,-CV_PI/2,CV_PI/2,-CV_PI/2,CV_PI/2);
}

void TLDK::init()

void TLD::track(const Mat& img1, const Mat& img2,const vector<Point2f> &points1, vector<Point2f> &points2){
  //Frame-to-frame tracking with forward-backward error cheking
  tracker.trackf2f(img1,img2,points1,points2);
}

/*
void TLD::generate(){
  Mat warpbuf, warped;
  Mat matM(2, 3, CV_64F);
  RNG& random_ng = theRNG();
  patchGenerator.generateRandomTransform(Point2f(), Point2f(), matM, random_ng);
  patchGenerator.warpWholeImage(last_gray, matM, warpbuf, warped,0, random_ng);
  imwrite("warped.jpg",warped);
}
*/

