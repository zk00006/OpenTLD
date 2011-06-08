#include <opencv2/opencv.hpp>
#pragma once

/*
struct tld_params{
  int min_win;
  int patch_size;
};
/s
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

void drawPoints(cv::Mat& image, vector<cv::Point2f> points);

cv::Mat createMask(const cv::Mat& image, CvRect box);

float median(vector<float> v);
