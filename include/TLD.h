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
  //Training data
  vector<pair<vector<int>,int> > pX; //positive ferns <features,labels=1>
  vector<pair<vector<int>,int> > nX; // negative ferns <features,labels=0>
  cv::Mat pEx;  //positive NN example
  vector<cv::Mat> nEx; //negative NN examples
  //Test data
  vector<pair<vector<int>,int> > nXT; //negative data to Test
  vector<cv::Mat> nExT; //negative NN examples to Test
  float var;

public:
//Constructors
  TLD();
  TLD(const cv::FileNode& file);
  void read(const cv::FileNode& file);
//Last frame data
  BoundingBox lastbox;
  bool lastvalid;
  float lastconf;
//Current frame data
  //Tracker data
  BoundingBox tbb;
  bool tvalid;
  float tconf;
  //Detector data
  vector<BoundingBox> dbb;
  vector<bool> dvalid;
  vector<float> dconf;
  bool detected;

//Bounding Boxes
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
  void bbPoints(vector<cv::Point2f>& points, const BoundingBox& bb,int pts,int margin);
  void bbPredict(const vector<cv::Point2f>& points1,const vector<cv::Point2f>& points2,
                 const BoundingBox& bb1,BoundingBox& bb2);
  float getVar(const BoundingBox& box,const cv::Mat& sum,const cv::Mat& sqsum);

  bool bbComp(const BoundingBox& bb1,const BoundingBox& bb2);
 //Methods
  void init(const cv::Mat& frame1,const cv::Rect &box);
  void generatePositiveData(const cv::Mat& frame, const cv::PatchGenerator& patchGenerator);
  void generateNegativeData(const cv::Mat& frame, const cv::PatchGenerator& patchGenerator);

  void processFrame(const cv::Mat& img1,const cv::Mat& img2,vector<cv::Point2f>& points,
                    BoundingBox& bbnext,bool& lastboxfound);
  void track(const cv::Mat& img1, const cv::Mat& img2,vector<cv::Point2f>& points2);
  void detect(const cv::Mat& frame);
  void clusterConf(const vector<BoundingBox>& dbb,const vector<float>& dconf,vector<BoundingBox>& cbb,vector<float>& cconf);
  void evaluate();//compare tacked pts with detected pts
  void learn(const Mat& img); // lear from correct detections and from errors

};


