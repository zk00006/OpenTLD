/*
 * TLD.cpp
 *
 *  Created on: Jun 9, 2011
 *      Author: alantrrs
 */

#include <TLD.h>
#include <stdio.h>
using namespace cv;
using namespace std;


TLD::TLD()
{
}
TLD::TLD(const FileNode& file){
  read(file);
}

void TLD::read(const FileNode& file){
  ///Bounding Box Parameters
  min_win = (int)file["min_win"];
  ///Genarator Parameters
  //initial parameters for positive examples
  patch_size = (int)file["patch_size"];
  num_closest_init = (int)file["num_closest_init"];
  num_warps_init = (int)file["num_warps_init"];
  noise_init = (int)file["noise_init"];
  angle_init = (float)file["angle_init"];
  shift_init = (float)file["shift_init"];
  scale_init = (float)file["scale_init"];
  //update parameters for positive examples
  num_closest_update = (int)file["num_closest_update"];
  num_warps_update = (int)file["num_warps_update"];
  noise_update = (int)file["noise_update"];
  angle_update = (float)file["angle_update"];
  shift_update = (float)file["shift_update"];
  scale_update = (float)file["scale_update"];
  //parameters for negative examples
  bad_overlap = (float)file["overlap"];
  bad_patches = (int)file["num_patches"];
  classifier.read(file);
}

void TLD::init(const Mat& frame1,const Rect& box,FILE* bb_file){
  //bb_file = fopen("bounding_boxes.txt","w");
  //Get Bounding Boxes
    buildGrid(frame1,box);
    printf("Created %d bounding boxes\n",(int)grid.size());
  ///Preparation
  //allocation
  iisum.create(frame1.rows+1,frame1.cols+1,CV_32F);
  iisqsum.create(frame1.rows+1,frame1.cols+1,CV_64F);
  dconf.reserve(100);
  dbb.reserve(100);
  bbox_step =7;
  //tmp.conf.reserve(grid.size());
  tmp.conf = vector<float>(grid.size());
  tmp.patt = vector<vector<int> >(grid.size(),vector<int>(10,0));
  //tmp.patt.reserve(grid.size());
  dt.bb.reserve(grid.size());
  good_boxes.reserve(grid.size());
  bad_boxes.reserve(grid.size());
  pEx.create(patch_size,patch_size,CV_64F);
  //Init Generator
  generator = PatchGenerator (0,0,noise_init,true,1-scale_init,1+scale_init,-angle_init*CV_PI/180,angle_init*CV_PI/180,-angle_init*CV_PI/180,angle_init*CV_PI/180);
  getOverlappingBoxes(box,num_closest_init);
  printf("Found %d good boxes, %d bad boxes\n",(int)good_boxes.size(),(int)bad_boxes.size());
  printf("Best Box: %d %d %d %d\n",best_box.x,best_box.y,best_box.width,best_box.height);
  printf("Bounding box hull: %d %d %d %d\n",bbhull.x,bbhull.y,bbhull.width,bbhull.height);
  //Correct Bounding Box
  lastbox=best_box;
  lastconf=1;
  lastvalid=true;
  //Print
  fprintf(bb_file,"%d,%d,%d,%d,%f\n",lastbox.x,lastbox.y,lastbox.br().x,lastbox.br().y,lastconf);
  //Prepare Classifier
  classifier.prepare(scales);
  ///Generate Data
  // Generate positive data
  generatePositiveData(frame1,num_warps_init);
  // Set variance threshold
  Scalar stdev, mean;
  meanStdDev(frame1(best_box),mean,stdev);
  integral(frame1,iisum,iisqsum);
  var = pow(stdev.val[0],2)*0.5; //getVar(best_box,iisum,iisqsum);
  cout << "variance: " << var << endl;
  //check variance
  double vr =  getVar(best_box,iisum,iisqsum)*0.5;
  cout << "check variance: " << vr << endl;
  // Generate negative data
  generateNegativeData(frame1);
  //Split Negative Ferns into Training and Testing sets (they are already shuffled)
  int half = (int)nX.size()*0.5f;
  nXT.assign(nX.begin()+half,nX.end());
  nX.resize(half);
  ///Split Negative NN Examples into Training and Testing sets
  half = (int)nEx.size()*0.5f;
  nExT.assign(nEx.begin()+half,nEx.end());
  nEx.resize(half);
  //Merge Negative Data with Positive Data and shuffle it
  vector<pair<vector<int>,int> > ferns_data(nX.size()+pX.size());
  vector<int> idx = index_shuffle(0,ferns_data.size());
  int a=0;
  for (int i=0;i<pX.size();i++){
      ferns_data[idx[a]] = pX[i];
      a++;
  }
  for (int i=0;i<nX.size();i++){
      ferns_data[idx[a]] = nX[i];
      a++;
  }
  //Data already have been shuffled, just putting it in the same vector
  vector<cv::Mat> nn_data(nEx.size()+1);
  nn_data[0] = pEx;
  for (int i=0;i<nEx.size();i++){
      nn_data[i+1]= nEx[i];
  }
  ///Training
  classifier.trainF(ferns_data,2); //bootstrap = 2
  classifier.trainNN(nn_data);
  ///Threshold Evaluation on testing sets
  classifier.evaluateTh(nXT,nExT);
}

/* Generate Positive data
 * Inputs:
 * - good_boxes (bbP)
 * - best_box (bbP0)
 * - frame (im0)
 * Outputs:
 * - Positive fern features (pX)
 * - Positive NN examples (pEx)
 */
void TLD::generatePositiveData(const Mat& frame, int num_warps){
  Scalar mean;
  Scalar stdev;
  getPattern(frame(best_box),pEx,mean,stdev);
  //Get Fern features on warped patches
  Mat img;
  Mat warped;
  GaussianBlur(frame,img,Size(9,9),1.5);
  warped = img(bbhull);
  RNG& rng = theRNG();
  Point2f pt(bbhull.x+(bbhull.width-1)*0.5f,bbhull.y+(bbhull.height-1)*0.5f);
  vector<int> fern(classifier.getNumStructs());
  pX.clear();
  Mat patch;
  if (pX.capacity()<num_warps*good_boxes.size())
    pX.reserve(num_warps*good_boxes.size());
  int idx;
  for (int i=0;i<num_warps;i++){
     if (i>0)
       generator(frame,pt,warped,bbhull.size(),rng);
       for (int b=0;b<good_boxes.size();b++){
         idx=good_boxes[b];
		 patch = img(grid[idx]);
         classifier.getFeatures(patch,grid[idx].sidx,fern);
         pX.push_back(make_pair(fern,1));
     }
  }
  printf("Positive examples generated: ferns:%d NN:1\n",(int)pX.size());
}

void TLD::getPattern(const Mat& img, Mat& pattern,Scalar& mean,Scalar& stdev){
  //Output: resized Zero-Mean patch
  resize(img,pattern,Size(patch_size,patch_size));
  meanStdDev(pattern,mean,stdev);
  pattern.convertTo(pattern,CV_32F);
  pattern = pattern-mean.val[0];
}

void TLD::generateNegativeData(const Mat& frame){
/* Inputs:
 * - Image
 * - bad_boxes (Boxes far from the bounding box)
 * - variance (pEx variance)
 * Outputs
 * - Negative fern features (nX)
 * - Negative NN examples (nEx)
 */
  random_shuffle(bad_boxes.begin(),bad_boxes.end());//Random shuffle bad_boxes indexes
  int idx;
  //Get Fern Features of the boxes with big variance (calculated using integral images)
  int a=0;
  //int num = std::min((int)bad_boxes.size(),(int)bad_patches*100); //limits the size of bad_boxes to try
  printf("negative data generation started.\n");
  vector<int> fern(classifier.getNumStructs());
  nX.reserve(bad_boxes.size());
  Mat patch;
  for (int j=0;j<bad_boxes.size();j++){
      idx = bad_boxes[j];
          if (getVar(grid[idx],iisum,iisqsum)<var*0.5f)
            continue;
      patch =  frame(grid[idx]);
	  classifier.getFeatures(patch,grid[idx].sidx,fern);
      nX.push_back(make_pair(fern,0));
      a++;
  }
  printf("Negative examples generated: ferns: %d ",a);
  //random_shuffle(bad_boxes.begin(),bad_boxes.begin()+bad_patches);//Randomly selects 'bad_patches' and get the patterns for NN;
  Scalar dum1, dum2;
  nEx=vector<Mat>(bad_patches);
  for (int i=0;i<bad_patches;i++){
      idx=bad_boxes[i];
	  patch = frame(grid[idx]);
      getPattern(patch,nEx[i],dum1,dum2);
  }
  printf("NN: %d\n",(int)nEx.size());
}

double TLD::getVar(const BoundingBox& box,const Mat& sum,const Mat& sqsum){
  double brs = sum.at<int>(box.y+box.height,box.x+box.width);
  double bls = sum.at<int>(box.y+box.height,box.x);
  double trs = sum.at<int>(box.y,box.x+box.width);
  double tls = sum.at<int>(box.y,box.x);
  double brsq = sqsum.at<double>(box.y+box.height,box.x+box.width);
  double blsq = sqsum.at<double>(box.y+box.height,box.x);
  double trsq = sqsum.at<double>(box.y,box.x+box.width);
  double tlsq = sqsum.at<double>(box.y,box.x);
  double mean = (brs+tls-trs-bls)/((double)box.area());
  double sqmean = (brsq+tlsq-trsq-blsq)/((double)box.area());
  return sqmean-mean*mean;
}

void TLD::processFrame(const cv::Mat& img1,const cv::Mat& img2,vector<Point2f>& points1,vector<Point2f>& points2,BoundingBox& bbnext,bool& lastboxfound, bool tl, FILE* bb_file){
  vector<BoundingBox> cbb;
  vector<float> cconf;
  int confident_detections=0;
  int didx; //detection index
  ///Track
  if(lastboxfound && tl){
      track(img1,img2,points1,points2);
  }
  else{
      tracked = false;
  }
  ///Detect
  detect(img2);
  ///Integration
  if (tracked){
      bbnext=tbb;
      lastconf=tconf;
      lastvalid=tvalid;
      printf("Tracked\n");
      if(detected){                                               //   if Detected
          clusterConf(dbb,dconf,cbb,cconf);                       //   cluster detections
          printf("Found %d clusters\n",(int)cbb.size());
          for (int i=0;i<cbb.size();i++){
              if (bbOverlap(tbb,cbb[i])<0.5 && cconf[i]>tconf){  //  Get index of a clusters that is far from tracker and are more confident than the tracker
                  confident_detections++;
                  didx=i; //detection index
              }
          }
          if (confident_detections==1){                                //if there is ONE such a cluster, re-initialize the tracker
              printf("Found a better match..reinitializing tracking\n");
              bbnext=cbb[didx];
              lastconf=cconf[didx];
              lastvalid=false;
          }
          else {
              printf("%d confident cluster was found\n",confident_detections);
              int cx=0,cy=0,cw=0,ch=0;
              int close_detections=0;
              for (int i=0;i<dbb.size();i++){
                  if(bbOverlap(tbb,dbb[i])>0.7){                     // Get mean of close detections
                      cx += dbb[i].x;
                      cy +=dbb[i].y;
                      cw += dbb[i].width;
                      ch += dbb[i].height;
                      close_detections++;
                      printf("weighted detection: %d %d %d %d\n",dbb[i].x,dbb[i].y,dbb[i].width,dbb[i].height);
                  }
              }
              if (close_detections>0){
                  bbnext.x = cvRound((float)(10*tbb.x+cx)/(float)(10+close_detections));   // weighted average trackers trajectory with the close detections
                  bbnext.y = cvRound((float)(10*tbb.y+cy)/(float)(10+close_detections));
                  bbnext.width = cvRound((float)(10*tbb.width+cw)/(float)(10+close_detections));
                  bbnext.height =  cvRound((float)(10*tbb.height+ch)/(float)(10+close_detections));
                  printf("Tracker bb: %d %d %d %d\n",tbb.x,tbb.y,tbb.width,tbb.height);
                  printf("Average bb: %d %d %d %d\n",bbnext.x,bbnext.y,bbnext.width,bbnext.height);
                  printf("Weighting %d close detection(s) with tracker..\n",close_detections);
              }
              else{
                printf("%d close detections were found\n",close_detections);

              }
          }
      }
  }
  else{                                       //   If NOT tracking
      printf("Not tracking..\n");
      lastboxfound = false;
      lastvalid = false;
      if(detected){                           //  and detector is defined
          clusterConf(dbb,dconf,cbb,cconf);   //  cluster detections
          printf("Found %d clusters\n",(int)cbb.size());
          if (cconf.size()==1){
              bbnext=cbb[0];
              lastconf=cconf[0];
              printf("Confident detection..reinitializing tracker\n");
              lastboxfound = true;
          }
      }
  }
  lastbox=bbnext;
  if (lastboxfound)
    fprintf(bb_file,"%d,%d,%d,%d,%f\n",lastbox.x,lastbox.y,lastbox.br().x,lastbox.br().y,lastconf);
  else
    fprintf(bb_file,"NaN,NaN,NaN,NaN,NaN\n");
  if (lastvalid && tl)
    learn(img2);
}


void TLD::track(const Mat& img1, const Mat& img2,vector<Point2f>& points1,vector<Point2f>& points2){
  /*Inputs:
   * -current frame(img2), last frame(img1), last Bbox(bbox_f[0]).
   *Outputs:
   *- Confidence(tconf), Predicted bounding box(tbb),Validity(tvalid), points2 (for display purposes only)
   */
  //Generate points
  bbPoints(points1,lastbox);
  if (points1.size()<1){
      printf("BB= %d %d %d %d, Points not generated\n",lastbox.x,lastbox.y,lastbox.width,lastbox.height);
      tvalid=false;
      tracked=false;
      return;
  }
  vector<Point2f> points = points1;
  //Frame-to-frame tracking with forward-backward error cheking
  tracked = tracker.trackf2f(img1,img2,points,points2);
  if (tracked){
      //Bounding box prediction
      bbPredict(points,points2,lastbox,tbb);
      if (tracker.getFB()>10 || tbb.x>img2.cols ||  tbb.y>img2.rows || tbb.br().x < 1 || tbb.br().y <1){
          tvalid =false; //too unstable prediction or bounding box out of image
          tracked = false;
          printf("Too unstable predictions FB error=%f\n",tracker.getFB());
          return;
      }
      //Estimate Confidence and Validity
      Mat pattern;
      Scalar mean, stdev;
      BoundingBox bb;
      bb.x = max(tbb.x,0);
      bb.y = max(tbb.y,0);
      bb.width = min(min(img2.cols-tbb.x,tbb.width),min(tbb.width,tbb.br().x));
      bb.height = min(min(img2.rows-tbb.y,tbb.height),min(tbb.height,tbb.br().y));
      getPattern(img2(bb),pattern,mean,stdev);
      vector<int> isin;
      float dummy;
      classifier.NNConf(pattern,isin,dummy,tconf); //Conservative Similarity
      tvalid = lastvalid;
      if (tconf>classifier.thr_nn_valid){
          tvalid =true;
      }
  }
  else
    printf("No points tracked\n");

}

void TLD::bbPoints(vector<cv::Point2f>& points,const BoundingBox& bb){
  int max_pts=10;
  int margin_h=0;
  int margin_v=0;
  int stepx = ceil((bb.width-2*margin_h)/max_pts);
  int stepy = ceil((bb.height-2*margin_v)/max_pts);
  for (int y=bb.y+margin_v;y<bb.y+bb.height-margin_v;y+=stepy){
      for (int x=bb.x+margin_h;x<bb.x+bb.width-margin_h;x+=stepx){
          points.push_back(Point2f(x,y));
      }
  }
}

void TLD::bbPredict(const vector<cv::Point2f>& points1,const vector<cv::Point2f>& points2,
                    const BoundingBox& bb1,BoundingBox& bb2)    {
  int npoints = (int)points1.size();
  vector<float> xoff(npoints);
  vector<float> yoff(npoints);
  printf("tracked points : %d\n",npoints);
  for (int i=0;i<npoints;i++){
      xoff[i]=points2[i].x-points1[i].x;
      yoff[i]=points2[i].y-points1[i].y;
  }
  float dx = median(xoff);
  float dy = median(yoff);
  float s;
  if (npoints>1){
      vector<float> d;
      d.reserve(npoints*(npoints-1)/2);
      for (int i=0;i<npoints;i++){
          for (int j=i+1;j<npoints;j++){
              d.push_back(norm(points2[i]-points2[j])/norm(points1[i]-points1[j]));
          }
      }
      s = median(d);
  }
  else {
      s = 1.0;
  }
  float s1 = 0.5*(s-1)*bb1.width;
  float s2 = 0.5*(s-1)*bb1.height;
  printf("s= %f s1= %f s2= %f \n",s,s1,s2);
  bb2.x = round( bb1.x + dx -s1);
  bb2.y = round( bb1.y + dy -s2);
  bb2.width = round(bb1.width*s);
  bb2.height = round(bb1.height*s);
  printf("predicted bb: %d %d %d %d\n",bb2.x,bb2.y,bb2.br().x,bb2.br().y);
}

void TLD::detect(const cv::Mat& frame){
  //cleaning
  dbb.clear();
  dconf.clear();
  dt.bb.clear();
  double t = (double)getTickCount();
  Mat img(frame.rows,frame.cols,CV_8U);
  integral(frame,iisum,iisqsum);
  GaussianBlur(frame,img,Size(9,9),1.5);
  int numtrees = classifier.getNumStructs();
  float fern_th = classifier.getFernTh();
  vector <int> ferns(10);
  float conf;
  int a=0;
  Mat patch;
  for (int i=0;i<grid.size();i++){//FIXME: BottleNeck
      if (getVar(grid[i],iisum,iisqsum)>=var){
          a++;
		  patch = img(grid[i]);
          classifier.getFeatures(patch,grid[i].sidx,ferns);
          conf = classifier.measure_forest(ferns);
          tmp.conf[i]=conf;
          tmp.patt[i]=ferns;
          if (conf>numtrees*fern_th){
              dt.bb.push_back(i);
          }
      }
      else
        tmp.conf[i]=0.0;
  }
  int detections = dt.bb.size();
  printf("%d Bounding boxes passed the variance filter\n",a);
  printf("%d Initial detection from Fern Classifier\n",detections);
  if (detections>100){
      nth_element(dt.bb.begin(),dt.bb.begin()+100,dt.bb.end(),CComparator(tmp.conf));
      dt.bb.resize(100);
      detections=100;
  }
//  for (int i=0;i<detections;i++){
//        drawBox(img,grid[dt.bb[i]]);
//    }
//  imshow("detections",img);
  if (detections==0){
        detected=false;
        return;
      }
  printf("Fern detector made %d detections ",detections);
  t=(double)getTickCount()-t;
  printf("in %gms\n", t*1000/getTickFrequency());
                                                                       //  Initialize detection structure
  dt.patt = vector<vector<int> >(detections,vector<int>(10,0));        //  Corresponding codes of the Ensemble Classifier
  dt.conf1 = vector<float>(detections);                                //  Relative Similarity (for final nearest neighbour classifier)
  dt.conf2 =vector<float>(detections);                                 //  Conservative Similarity (for integration with tracker)
  dt.isin = vector<vector<int> >(detections,vector<int>(3,-1));        //  Detected (isin=1) or rejected (isin=0) by nearest neighbour classifier
  dt.patch = vector<Mat>(detections,Mat(patch_size,patch_size,CV_32F));//  Corresponding patches
  int idx;
  Scalar mean, stdev;
  float nn_th = classifier.getNNTh();
  for (int i=0;i<detections;i++){                                         //  for every remaining detection
      idx=dt.bb[i];                                                       //  Get the detected bounding box index
	  patch = frame(grid[idx]);
      getPattern(patch,dt.patch[i],mean,stdev);                //  Get pattern within bounding box
      classifier.NNConf(dt.patch[i],dt.isin[i],dt.conf1[i],dt.conf2[i]);  //  Evaluate nearest neighbour classifier
      dt.patt[i]=tmp.patt[idx];
      //printf("Testing feature %d, conf:%f isin:(%d|%d|%d)\n",i,dt.conf1[i],dt.isin[i][0],dt.isin[i][1],dt.isin[i][2]);
      if (dt.conf1[i]>nn_th){                                               //  idx = dt.conf1 > tld.model.thr_nn; % get all indexes that made it through the nearest neighbour
          dbb.push_back(grid[idx]);                                         //  BB    = dt.bb(:,idx); % bounding boxes
          dconf.push_back(dt.conf2[i]);                                     //  Conf  = dt.conf2(:,idx); % conservative confidences
      }
  }                                                                         //  end
  if (dbb.size()>0){
      printf("Found %d NN matches\n",(int)dbb.size());
      detected=true;
  }
  else{
      printf("No NN matches found.\n");
      detected=false;
  }
}

void TLD::evaluate(){
}

void TLD::learn(const Mat& img){
  printf("[Learning] ");
  ///Check consistency
  BoundingBox bb;
  bb.x = max(lastbox.x,0);
  bb.y = max(lastbox.y,0);
  bb.width = min(min(img.cols-lastbox.x,lastbox.width),min(lastbox.width,lastbox.br().x));
  bb.height = min(min(img.rows-lastbox.y,lastbox.height),min(lastbox.height,lastbox.br().y));
  Scalar mean, stdev;
  Mat pattern;
  getPattern(img(bb),pattern,mean,stdev);
  vector<int> isin;
  float dummy, conf;
  classifier.NNConf(pattern,isin,conf,dummy);
  if (conf<0.5) {
      printf("Fast change..not training\n");
      lastvalid =false;
      return;
  }
  if (pow(stdev.val[0],2)<var){
      printf("Low variance..not training\n");
      lastvalid=false;
      return;
  }
  if(isin[2]==1){
      printf("Patch in negative data..not traing");
      lastvalid=false;
      return;
  }
/// Data generation
  for (int i=0;i<grid.size();i++){
      grid[i].overlap = bbOverlap(lastbox,grid[i]);
  }
  vector<pair<vector<int>,int> > fern_examples;
  good_boxes.clear();
  bad_boxes.clear();
  getOverlappingBoxes(lastbox,num_closest_update);
  if (good_boxes.size()>0)
    generatePositiveData(img,num_warps_update);
  else{
    lastvalid = false;
    printf("No good boxes..Not training");
    return;
  }
  fern_examples.reserve(pX.size()+bad_boxes.size());
  fern_examples.assign(pX.begin(),pX.end());
  int idx;
  for (int i=0;i<bad_boxes.size();i++){
      idx=bad_boxes[i];
      if (tmp.conf[idx]>=1){
          fern_examples.push_back(make_pair(tmp.patt[idx],0));
      }
  }
  vector<Mat> nn_examples;
  nn_examples.reserve(dt.bb.size()+1);
  nn_examples.push_back(pEx);
  for (int i=0;i<dt.bb.size();i++){
      idx = dt.bb[i];
      if (bbOverlap(lastbox,grid[idx]) < bad_overlap)
        nn_examples.push_back(dt.patch[i]);
  }
  /// Classifiers update
  classifier.trainF(fern_examples,2);
  classifier.trainNN(nn_examples);
  classifier.show();
}

void TLD::buildGrid(const cv::Mat& img, const cv::Rect& box){
  const float SHIFT = 0.1;
  const float SCALES[] = {0.16151,0.19381,0.23257,0.27908,0.33490,0.40188,0.48225,
                          0.57870,0.69444,0.83333,1,1.20000,1.44000,1.72800,
                          2.07360,2.48832,2.98598,3.58318,4.29982,5.15978,6.19174};
  int width, height, min_bb_side;
  //Rect bbox;
  BoundingBox bbox;
  Size scale;
  int sc=0;
  for (int s=0;s<21;s++){
    width = round(box.width*SCALES[s]);
    height = round(box.height*SCALES[s]);
    min_bb_side = min(height,width);
    if (min_bb_side < min_win || width > img.cols || height > img.rows)
      continue;
    scale.width = width;
    scale.height = height;
    scales.push_back(scale);
    for (int y=1;y<img.rows-height;y+=round(SHIFT*min_bb_side)){
      for (int x=1;x<img.cols-width;x+=round(SHIFT*min_bb_side)){
        bbox.x = x;
        bbox.y = y;
        bbox.width = width;
        bbox.height = height;
        bbox.overlap = bbOverlap(bbox,BoundingBox(box));
        bbox.sidx = sc;
        grid.push_back(bbox);
      }
    }
    sc++;
  }
}

float TLD::bbOverlap(const BoundingBox& box1,const BoundingBox& box2){
  if (box1.x > box2.x+box2.width) { return 0.0; }
  if (box1.y > box2.y+box2.height) { return 0.0; }
  if (box1.x+box1.width < box2.x) { return 0.0; }
  if (box1.y+box1.height < box2.y) { return 0.0; }

  float colInt =  min(box1.x+box1.width,box2.x+box2.width) - max(box1.x, box2.x);
  float rowInt =  min(box1.y+box1.height,box2.y+box2.height) - max(box1.y,box2.y);

  float intersection = colInt * rowInt;
  float area1 = box1.width*box1.height;
  float area2 = box2.width*box2.height;
  return intersection / (area1 + area2 - intersection);
}

void TLD::getOverlappingBoxes(const cv::Rect& box1,int num_closest){
  float max_overlap = 0;
  for (int i=0;i<grid.size();i++){
      if (grid[i].overlap > max_overlap) {
          max_overlap = grid[i].overlap;
          best_box = grid[i];
      }
      if (grid[i].overlap > 0.6){
          good_boxes.push_back(i);
      }
      else if (grid[i].overlap < bad_overlap){
          bad_boxes.push_back(i);
      }
  }
  //Get the best num_closest (10) boxes and puts them in good_boxes
  if (good_boxes.size()>num_closest){
    std::nth_element(good_boxes.begin(),good_boxes.begin()+num_closest,good_boxes.end(),OComparator(grid));
    good_boxes.resize(num_closest);
  }
  getBBHull();
}

void TLD::getBBHull(){
  int x1=INT_MAX, x2=0;
  int y1=INT_MAX, y2=0;
  int idx;
  for (int i=0;i<good_boxes.size();i++){
      idx= good_boxes[i];
      x1=min(grid[idx].x,x1);
      y1=min(grid[idx].y,y1);
      x2=max(grid[idx].x+grid[idx].width,x2);
      y2=max(grid[idx].y+grid[idx].height,y2);
  }
  bbhull.x = x1;
  bbhull.y = y1;
  bbhull.width = x2-x1;
  bbhull.height = y2 -y1;
}

bool bbcomp(const BoundingBox& b1,const BoundingBox& b2){
  TLD t;
    if (t.bbOverlap(b1,b2)<0.5)
      return false;
    else
      return true;
}
int TLD::clusterBB(const vector<BoundingBox>& dbb,vector<int>& indexes){
  //FIXME: Conditional jump or move depends on uninitialised value(s)
  const int c = dbb.size();
  //1. Build proximity matrix
  Mat D(c,c,CV_32F);
  float d;
  for (int i=0;i<c;i++){
      for (int j=i+1;j<c;j++){
        d = 1-bbOverlap(dbb[i],dbb[j]);
        D.at<float>(i,j) = d;
        D.at<float>(j,i) = d;
      }
  }
  //2. Initialize disjoint clustering
 float L[c-1]; //Level
 int nodes[c-1][2];
 int belongs[c];
 int m=c;
 for (int i=0;i<c;i++){
    belongs[i]=i;
 }
 for (int it=0;it<c-1;it++){
 //3. Find nearest neighbor
     float min_d = 1;
     int node_a, node_b;
     for (int i=0;i<D.rows;i++){
         for (int j=i+1;j<D.cols;j++){
             if (D.at<float>(i,j)<min_d && belongs[i]!=belongs[j]){
                 min_d = D.at<float>(i,j);
                 node_a = i;
                 node_b = j;
             }
         }
     }
     if (min_d>0.5){
         int max_idx =0;
         bool visited;
         for (int j=0;j<c;j++){
             visited = false;
             for(int i=0;i<2*c-1;i++){
                 if (belongs[j]==i){
                     indexes[j]=max_idx;
                     visited = true;
                 }
             }
             if (visited)
               max_idx++;
         }
         return max_idx;
     }

 //4. Merge clusters and assign level
     L[m]=min_d;
     nodes[it][0] = belongs[node_a];
     nodes[it][1] = belongs[node_b];
     for (int k=0;k<c;k++){
         if (belongs[k]==belongs[node_a] || belongs[k]==belongs[node_b])
           belongs[k]=m;
     }
     m++;
 }
 return 1;

}

void TLD::clusterConf(const vector<BoundingBox>& dbb,const vector<float>& dconf,vector<BoundingBox>& cbb,vector<float>& cconf){
  int numbb =dbb.size();
  vector<int> T;
  float space_thr = 0.5;
  int c=1;
  switch (numbb){
  case 1:
    cbb=vector<BoundingBox>(1,dbb[0]);
    cconf=vector<float>(1,dconf[0]);
    return;
    break;
  case 2:
    T =vector<int>(2,0);
    if (1-bbOverlap(dbb[0],dbb[1])>space_thr){
      T[1]=1;
      c=2;
    }
    break;
  default:
    T = vector<int>(numbb,0);
    c = partition(dbb,T,(*bbcomp));
    //c = clusterBB(dbb,T);
    break;
  }
  cconf=vector<float>(c);
  cbb=vector<BoundingBox>(c);
  printf("Cluster indexes: ");
  BoundingBox bx;
  for (int i=0;i<c;i++){
      float cnf=0;
      int N=0,mx=0,my=0,mw=0,mh=0;
      for (int j=0;j<T.size();j++){
          if (T[j]==i){
              printf("%d ",i);
              cnf=cnf+dconf[j];
              mx=mx+dbb[j].x;
              my=my+dbb[j].y;
              mw=mw+dbb[j].width;
              mh=mh+dbb[j].height;
              N++;
          }
      }
      if (N>0){
          cconf[i]=cnf/N;
          bx.x=cvRound(mx/N);
          bx.y=cvRound(my/N);
          bx.width=cvRound(mw/N);
          bx.height=cvRound(mh/N);
          cbb[i]=bx;
      }
  }
  printf("\n");
}

