#include <opencv2/opencv.hpp>
#include <LKTracker.h>

class TLD{
private:
  cv::PatchGenerator patchGenerator;  
  cv::FernClassifier classifier;
  LKTracker tracker;
public:
  TLD();
  void init(const cv::Mat& frame1,const vector<cv::Point2f> &points1);
  void track(const cv::Mat& img1, const cv::Mat& img2,const vector<cv::Point2f> &points1, vector<cv::Point2f> &points2);
  //void detect(const cv::Mat& frame, vector<cv::Point2f> &detected_pts);
};