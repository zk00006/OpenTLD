#include <opencv2/opencv.hpp>
#pragma once


void drawBox(cv::Mat& image, CvRect box, cv::Scalar color = cvScalarAll(255), int thick=1); 

void drawPoints(cv::Mat& image, vector<cv::Point2f> points);

cv::Mat createMask(const cv::Mat& image, CvRect box);

float median(vector<float> v);

bool comparator(const std::pair<float,cv::Rect>& item1,const std::pair<float,cv::Rect>& item2);

vector<int> index_shuffle(int begin,int end);
