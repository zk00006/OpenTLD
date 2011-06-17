#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <LKTracker.h>
#include <FerNNClassifier.h>


//Bounding Boxes
  struct BoundingBox : public cv::Rect {
    BoundingBox(){}
    BoundingBox(cv::Rect r): cv::Rect(r){}
    //cv::Rect bbox;
  public:
    float overlap;
    int sidx; //scale index
  };


class TLD{
private:
  FerNNClassifier classifier;
  LKTracker tracker;
  int min_win;
  int patch_size;
  float ncc_thesame;
  float valid;
  int num_trees;
  int num_features;
  float thr_fern;
  float thr_nn;
  float thr_nn_valid;
  //initial parameters for positive examples
  int num_closest_init;
  int num_warps_init;
  int noise_init;
  float angle_init;
  float shift_init;
  float scale_init;
  //update parameters for positive examples
  int num_closest_update;
  int num_warps_update;
  int noise_update;
  float angle_update;
  float shift_update;
  float scale_update;
  //parameters for negative examples
  float bad_overlap;
  float bad_patches;
  //members
  cv::Mat pEx;
  vector<cv::Mat> nEx;
  float var;
  vector<float> dconfidence;
  vector<float> tconfidence;
public:
  TLD();
  TLD(const cv::FileNode& file);
  void read(const cv::FileNode& file);

  vector<BoundingBox> grid;
  vector<cv::Size> scales;
  vector<BoundingBox> good_boxes; //bboxes with overlap > 0.6
  vector<BoundingBox> bad_boxes; //bboxes with overlap < 0.2
  BoundingBox bbhull; // hull of good_boxes
  BoundingBox best_box; // maximum overlapping bbox
  void buildGrid(const cv::Mat& img, const cv::Rect& box);
  float bbOverlap(const BoundingBox& box1,const BoundingBox& box2);
  void getOverlappingBoxes(const cv::Rect& box1,int num_closest);
  void getBBHull(const vector<BoundingBox>& boxes, BoundingBox& bbhull);
  void getPattern(const cv::Mat& img, cv::Mat& pattern,cv::Scalar& mean,cv::Scalar& stdev);

 //Methods
  void init(const cv::Mat& frame1,const cv::Rect &box);
  void generatePositiveData(const cv::Mat& frame, const cv::PatchGenerator& patchGenerator);
  void generateNegativeData(const cv::Mat& frame, const cv::PatchGenerator& patchGenerator);
  void trainFromSingleView(const cv::Mat& frame1, const cv::Rect& bbox, const vector<cv::KeyPoint>& kpts,vector<cv::Point2f>& pts);
  void track(const cv::Mat& img1, const cv::Mat& img2,const vector<cv::Point2f> &points1, vector<cv::Point2f> &points2);
  void detect(const cv::Mat& frame,vector<cv::KeyPoint>& points, vector<float>& confidence);
  void evaluate();//compare tacked pts with detected pts
  void learn(); // lear from correct detections and from errors
  vector<pair<vector<int>,int> > ferns; //ferns,labels

  //Other
  void splitKeyPoints(const vector<cv::KeyPoint>& kpts, const cv::Rect& bbox,vector<cv::KeyPoint>& good_pts,vector<cv::KeyPoint>& bad_pts);

};


