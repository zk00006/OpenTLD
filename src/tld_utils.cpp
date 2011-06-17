#include <tld_utils.h>
using namespace cv;

void drawBox(Mat& image, CvRect box, Scalar color, int thick){
  rectangle( image, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),color, thick);
} 

void drawPoints(Mat& image, vector<Point2f> points){
  for( vector<Point2f>::const_iterator i = points.begin(), ie = points.end(); i != ie; ++i )
      {
      Point center( cvRound(i->x ), cvRound(i->y));
      circle(image,*i,2,Scalar::all(255),1);
      }
}

Mat createMask(const Mat& image, CvRect box){
  Mat mask = Mat::zeros(image.rows,image.cols,CV_8U);
  drawBox(mask,box,Scalar::all(255),CV_FILLED);
  return mask;
}

float median(vector<float> v)
{
    int n = v.size() / 2;
    nth_element(v.begin(), v.begin()+n, v.end());
    return v[n];
}

bool comparator ( const pair<float,cv::Rect>& item1,const pair<float,cv::Rect>& item2){
  return item1.first > item2.first;
}

vector<int> index_shuffle(int begin,int end){
  vector<int> indexes;
  for (int i=begin;i<end;i++){
    indexes.push_back(i);
  }
  random_shuffle(indexes.begin(),indexes.end());
  return indexes;
}
