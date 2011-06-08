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
  float sim_med;
  vector<float> FB_error;
  float FB_med;
  cv::TermCriteria term_criteria;
  float lambda;
public:
  LKTracker();
  void trackf2f(const cv::Mat& img1, const cv::Mat& img2, vector<cv::Point2f> &points1, vector<cv::Point2f> &points2);
};

void calcFBError(const  vector<cv::Point2f> &pts1, const vector<cv::Point2f> &pts2, vector<float> &fberror);

void filterPts(vector<cv::Point2f>& points,vector<uchar>& status,vector<float>& fb_err,vector<float>& sim_err, float fbmed, float simed);
