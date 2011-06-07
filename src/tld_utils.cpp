#include <tld_utils.h>
using namespace cv;

void drawBox(Mat& image, CvRect box, Scalar color, int thick ){
  rectangle( image, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),color );
} 

Mat createMask(const Mat& image, CvRect box){
  Mat mask = Mat::zeros(image.rows,image.cols,CV_8U);
  drawBox(mask,box,Scalar(1,0,0),CV_FILLED);
  return mask;
}
