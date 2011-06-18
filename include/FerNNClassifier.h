/*
 * FerNNClassifier.h
 *
 *  Created on: Jun 14, 2011
 *      Author: alantrrs
 */

#include <opencv2/opencv.hpp>

class FerNNClassifier{
private:
  //Parameters
  float thr_fern;
  int structSize;
  int nstructs;
  float valid;
  float ncc_thesame;
  float thr_nn;
  float thr_nn_valid;

public:
  void read(const cv::FileNode& file);
  void prepare(const vector<cv::Size>& scales);
  void getFeatures(const cv::Mat& image,const cv::Rect& box, int scale_idx,vector<pair<vector<int>,int> >& ferns, int label);
  void update(vector<int> fern, int C, int N);
  float measure_forest(vector<int> fern);
  void trainF(const vector<pair<vector<int>,int> >& ferns,int resample);
  void trainNN(const vector<cv::Mat>& nn_examples);
  float NNConf(const cv::Mat& example,vector<int>& isin);
  void evaluateTh(const vector<pair<vector<int>,int> >& nXT,const vector<cv::Mat>& nExT);

  //Ferns Members
  struct Feature
      {
          uchar x1, y1, x2, y2;
          Feature() : x1(0), y1(0), x2(0), y2(0) {}
          Feature(int _x1, int _y1, int _x2, int _y2)
          : x1((uchar)_x1), y1((uchar)_y1), x2((uchar)_x2), y2((uchar)_y2)
          {}
          bool operator ()(const cv::Mat& patch) const
          { return patch.at<uchar>(y1,x1) > patch.at<uchar>(y2, x2); }
      };
  vector<vector<Feature> > features; //Ferns features (one vector for each scale)
  vector< vector<int> > nCounter; //negative counter
  vector< vector<int> > pCounter; //positive counter
  vector< vector<float> > posteriors; //Ferns posteriors
  float thrN; //Negative threshold
  float thrP;  //Positive thershold
  //NN Members
  vector<cv::Mat> pEx; //NN positive examples
  vector<cv::Mat> nEx; //NN negative examples
};
