#include <tld_utils.h>
using namespace cv;

void drawBox(Mat& image, CvRect box){
  rectangle( image, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),cvScalarAll(255) );
} 

