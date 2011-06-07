#include <opencv2/opencv.hpp>
/*
struct tld_params{
  int min_win;
  int patch_size;
};
struct tld_model{
  int min_win;
  int patch_size;
  int num_trees;
  int num_features;
  float fern_th;
  float nn_th;
};
*/

void drawBox(cv::Mat& image, CvRect box, cv::Scalar color = cvScalarAll(255), int thick=1); 

cv::Mat createMask(const cv::Mat& image, CvRect box);
