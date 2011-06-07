#include <LKTracker.h>
using namespace cv;
LKTracker::LKTracker(){
  term_criteria = TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 20, 0.03);
  window_size = Size(3,3);
  int level = 5;
}

void LKTracker::init(Mat img0, vector<Point2f> points){
  img1=img0;
  points1 = points;
  //Preallocate
  //pyr1 = Mat(img1.rows/3,img1.cols +8,CV_8U);
  //pyr2 = Mat(img1.rows/3,img1.cols +8,CV_8U);
  const int NUM_PTS = points.size();
  status = new char[NUM_PTS];
  track_error = new float[NUM_PTS];
  FB_error = new float[NUM_PTS];
}

void  LKTracker::trackf2f(Mat img){
  //Forward tracking
  //calcOpticalFlowPyrLK(img1,img2,points1,points2,);
  //oldstyle:cvCalcOpticalFlowPyrLK( &iimg1, &iimg2, pyr1, pyr1, points1, points2, points1.size(), window_size, level, status, track_error, term_criteria, CV_LKFLOW_INITIAL_GUESSES);
  //Backward tracking
  //oldstyle:cvCalcOpticalFlowPyrLK( img2, img1, pyr2, pyr1, points2, pointsFB, points2.size(),window_size, level, 0, 0, term_criteria, CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY );
  //Get Forward-Backward Error
  
  //Get Normal Cross-Correlation
  
  //Get Reliable Points
  //Swap images
  //iimg1 = iimg2;
  //img1 = Mat;
}
