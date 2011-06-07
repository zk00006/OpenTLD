#include <opencv2/opencv.hpp>
class LKTracker{
private:
  cv::Mat img1;
  cv::Mat img2;
  cv::Mat pyr1;
  cv::Mat pyr2;
  vector<cv::Point2f> points1;
  vector<cv::Point2f> points2;
  vector<cv::Point2f> pointsFB;
  cv::Size window_size;
  int level;
  char* status;
  float* track_error;
  float* FB_error;
  cv::TermCriteria term_criteria;
public:
  LKTracker();
  void init(cv::Mat img0,vector<cv::Point2f> points);
  void trackf2f(cv::Mat img);
};
