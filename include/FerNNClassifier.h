#include <opencv2/opencv.hpp>

class FerNNClassifier{
private:
  int structSize;
  int nstructs;
public:
  void prepare(int num_trees, int num_features,const vector<cv::Size>& scales);
  void getFeatures(const cv::Mat& image,const cv::Rect& box, int scale_idx);
  void trainFromSingleView();
  void evaluate();
  void update();


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
  vector<cv::Mat> pEx; //NN positive examples
  vector<cv::Mat> nEx; //NN negative examples
  vector<vector<Feature> > features; //Ferns features (one vector for each scale)
  vector<vector<int> > ferns; // Ferns measurments (one vector for each patch)
  vector< vector<int> > nCounter; //negative counter
  vector< vector<int> > pCounter; //positive counter
  vector< vector<int> > posteriors; //Ferns posteriors
};
