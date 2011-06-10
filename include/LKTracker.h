#include<tld_utils.h>
#include <opencv2/opencv.hpp>


class LKTracker{
private:
  vector<cv::Point2f> pointsFB;
  cv::Size window_size;
  int level;
  vector<uchar> status;
  vector<uchar> FB_status;
  vector<float> sim_error;
  vector<float> FB_error;
  cv::TermCriteria term_criteria;
  float lambda;
  void filterPts(vector<cv::Point2f>& points2);
public:
  LKTracker();
  void trackf2f(const cv::Mat& img1, const cv::Mat& img2, const vector<cv::Point2f> &points1, vector<cv::Point2f> &points2);
};


