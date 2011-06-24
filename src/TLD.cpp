/*
 * TLD.cpp
 *
 *  Created on: Jun 9, 2011
 *      Author: alantrrs
 */

#include <TLD.h>
using namespace cv;

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

void TLD::init(const Mat& frame1,const Rect& box){
  //Get Bounding Boxes
    buildGrid(frame1,box);
    printf("Created %d bounding boxes\n",(int)grid.size());
  ///Preparation
  //allocation
  iisum.create(frame1.rows+1,frame1.cols+1,CV_64F);
  iisqsum.create(frame1.rows+1,frame1.cols+1,CV_64F);
  dconf.reserve(100);
  dbb.reserve(100);
  tmpconf.reserve(grid.size());
  good_boxes.reserve(grid.size());
  bad_boxes.reserve(grid.size());
  pEx.create(patch_size,patch_size,CV_32F);
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
  //Prepare Classifier
  classifier.prepare(scales);
  ///Generate Data
  // Generate positive data
  generatePositiveData(frame1,num_warps_init);
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
  var =  pow(stdev.val[0],2)*0.5f;
  cout << "var: " << var << endl;
  //Get Fern features on warped patches
  Mat img;
  Mat warped;
  GaussianBlur(frame,img,Size(9,9),1.5);
  warped = img(bbhull);
  RNG& rng = theRNG();
  Point2f pt(bbhull.x+(bbhull.width-1)*0.5f,bbhull.y+(bbhull.height-1)*0.5f);
  vector<int> fern(classifier.getNumStructs());
  pX.clear();
  if (pX.capacity()<num_warps*good_boxes.size())
    pX.reserve(num_warps*good_boxes.size());
  for (int i=0;i<num_warps;i++){
     if (i>0)
       generator(frame,pt,warped,bbhull.size(),rng);
     for (int b=0;b<good_boxes.size();b++){
         classifier.getFeatures(img,good_boxes[b],good_boxes[b].sidx,fern);
         pX.push_back(make_pair(fern,1));
     }
  }
  printf("Positive fern examples generated: %d\n",(int)pX.size());
}

void TLD::getPattern(const Mat& img, Mat& pattern,Scalar& mean,Scalar& stdev){
  //Output: resized Zero-Mean patch
  resize(img,pattern,Size(patch_size,patch_size));
  meanStdDev(pattern,mean,stdev);
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
  vector<int> idx = index_shuffle(0,bad_boxes.size());//creates a random indexes from 0 to bad_boxes.size()
  //Get Fern Features of the boxes with big variance (calculated using integral images)
  integral(frame,iisum,iisqsum);
  int a=0;
  int num = std::min((int)bad_boxes.size(),(int)bad_patches*100); //limits the size of bad_boxes to try
  printf("negative data generation started.\n");
  vector<int> fern(classifier.getNumStructs());
  nX.reserve(num);
  for (int j=0;j<num;j++){
          if (getVar(bad_boxes[idx[j]],iisum,iisqsum)<var/2)
            continue;
      classifier.getFeatures(frame,bad_boxes[idx[j]],bad_boxes[idx[j]].sidx,fern);
      nX.push_back(make_pair(fern,0));
      a++;
  }
  printf("Negative examples generated: %d \n",a);
  //Randomly selects 'bad_patches' and get the patterns for NN;
  Mat negExample;
  Scalar dum1, dum2;
  nEx.reserve(bad_patches);
  for (int i=0;i<bad_patches;i++){
      getPattern(frame(bad_boxes[idx[i]]),negExample,dum1,dum2);
      nEx.push_back(negExample);
  }
}

float TLD::getVar(const BoundingBox& box,const Mat& sum,const Mat& sqsum){
  /* NOTE: Optimize variance calculation?
   * for quick initialization of small matrices and/or super-fast element access
   * double m[3][3] = {{a, b, c}, {d, e, f}, {g, h, i}};
   * cv::Mat M = cv::Mat(3, 3, CV_64F, m).inv();
   */
  /* #define ELEM(type,start,step,size,xpos,ypos) *((type*)(start+step*(ypos)+(xpos)*size))
   * double v = ELEM(float,myMatrix.data,myMatrix.step(),myMatrix.elemSize,x,y);    // Moderately speedy access
   * double v = ELEM(float,myMatrix.data,2560,4,x,y);                               // Even faster access
   */
  Point br(box.br().x+1,box.br().x+1);
  Point bl(box.x+1,box.y+box.height+1);
  Point tr(box.x +box.width+1,box.y+1);
  Point tl(box.x+1,box.y+1);
  float mean = (sum.at<double>(br)+sum.at<double>(tl)-sum.at<double>(tr)-sum.at<double>(bl))/box.area();
  float sqmean = (sqsum.at<double>(br)+sqsum.at<double>(tl)-sqsum.at<double>(tr)-sqsum.at<double>(bl))/box.area();
  return sqmean-mean*mean;
}

void TLD::processFrame(const cv::Mat& img1,const cv::Mat& img2,vector<Point2f>& points,BoundingBox& bbnext,bool& lastboxfound){
  vector<BoundingBox> cbb;
  vector<float> cconf;
  int confident_detections=0;
  int didx; //detection index
  vector<int> didxs; //detectionS indexeS
  //Track
  if(lastboxfound)
    track(img1,img2,points);
  //Detect
  detect(img2);
  if (tvalid){                                //    if TR % if tracker is defined
      bbnext=tbb;                             //        tld.bb(:,I)  = tBB;
      lastconf=tconf;                         //        tld.conf(I)  = tConf;
      lastvalid=tvalid;                       //        tld.valid(I) = tValid;
                                              //        tld.size(I)  = 1;
      printf("Tracking...\n");
      if(detected){                                              //   if DT % if detections are also defined
          clusterConf(dbb,dconf,cbb,cconf);                      //      [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
          printf("Found %d clusters\n",(int)cbb.size());
          for (int i=0;i<cbb.size();i++){
              if (bbOverlap(tbb,cbb[i])<0.5 && cconf[i]>tconf){  //      id = bb_overlap(tld.bb(:,I),cBB) < 0.5 & cConf > tld.conf(I); % get indexes of all clusters that are far from tracker and are more confident then the tracker
                  confident_detections++;
                  didx=i; //detection index
              }
          }
          if (confident_detections==1){                                //if sum(id) == 1 % if there is ONE such a cluster, re-initialize the tracker
              printf("Found a better match..reinitializing tracking\n");
              bbnext=cbb[didx];                                        //      tld.bb(:,I)  = cBB(:,id);
              lastconf=cconf[didx];                                    //      tld.conf(I)  = cConf(:,id);
                                                                       //      tld.size(I)  = cSize(:,id);
              lastvalid=0;                                             //      tld.valid(I) = 0;
          }
          else {                                                       //  else % othervide adjust the tracker's trajectory
              confident_detections=0;
              int cx=0,cy=0,cw=0,ch=0;
              for(int i=0;i<cbb.size();i++){
                  if(bbOverlap(tbb,cbb[i])>0.7){                        //      idTr = bb_overlap(tBB,tld.dt{I}.bb) > 0.7;  % get indexes of close detections
                      cx += cbb[i].x;
                      cy +=cbb[i].y;
                      cw += cbb[i].width;
                      ch += cbb[i].height;
                      confident_detections++;
                  }
              }
              if (confident_detections>0){
                  bbnext.x = (10*tbb.x+cx)/(10+confident_detections);   //     tld.bb(:,I) = mean([repmat(tBB,1,10) tld.dt{I}.bb(:,idTr)],2);  % weighted average trackers trajectory with the close detections
                  bbnext.y = (10*tbb.y+cy)/(10+confident_detections);
                  bbnext.width = (10*tbb.width+cw)/(10+confident_detections);
                  bbnext.height =  (10*tbb.height+ch)/(10+confident_detections);
                  printf("Weighting %d close cluster with tracker..\n",confident_detections);
              }
          }                                                             //     end
      }                                                                 //    end
  }
  else{                                       //    else % if tracker is not defined
      printf("Not tracking..\n");
      lastboxfound = false;
      if(detected){                           //        if DT % and detector is defined
          clusterConf(dbb,dconf,cbb,cconf);   //            [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
          printf("Found %d clusters\n",(int)cbb.size());
          if (cconf.size()==1){               //            if length(cConf) == 1 % and if there is just a single cluster, re-initalize the tracker
              bbnext=cbb[0];                  //                tld.bb(:,I)  = cBB;
              lastconf=cconf[0];              //                tld.conf(I)  = cConf;
                                              //                tld.size(I)  = cSize;
              lastvalid=0;                    //                tld.valid(I) = 0;
              printf("Confident detection..reinitializing tracker\n");
          lastboxfound = true;
          }                                   //            end
      }
                                              //        end
  }                                           //    end
  lastbox=bbnext;
}


void TLD::track(const Mat& img1, const Mat& img2,vector<Point2f>& points2){
  /*Inputs:
   * -current frame(img2), last frame(img1), last Bbox(bbox_f[0]).
   *Outputs:
   *- Confidence(tconf), Predicted bounding box(tbb),Validity(tvalid), points2 (for display purposes only)
   */
  //Generate points
  vector<Point2f> points1;
  bbPoints(points1,lastbox,10,5);
  if (points1.size()<=0){
      tvalid=false;
      return;
  }
  //Frame-to-frame tracking with forward-backward error cheking
  bool tracked=false;
  if (points1.size()>0)
    tracked = tracker.trackf2f(img1,img2,points1,points2);
  if (tracked){
      //Bounding box prediction
      bbPredict(points1,points2,lastbox,tbb);
      //printf("predicted bbox %d %d %d %d\n",tbb.tl().x,tbb.tl().y,tbb.br().x,tbb.br().y);
      if (tracker.getFB()>10 ||tbb.tl().x < 0 || tbb.tl().y<0 || tbb.br().x > img2.cols || tbb.br().y > img2.rows){
          tvalid =false; //too unstable prediction or bounding box out of image
          return;
      }
      //Estimate Confidence and Validity
      Mat pattern;
      Scalar mean, stdev;
      getPattern(img2(tbb),pattern,mean,stdev);
      vector<int> isin;
      float dummy;
      classifier.NNConf(pattern,isin,dummy,tconf); //Conservative Similarity
      tvalid = lastvalid;
      if (tconf>classifier.thr_nn_valid){
          tvalid =true;
      }
  }
  else
    tvalid = false;
}

void TLD::bbPredict(const vector<cv::Point2f>& points1,const vector<cv::Point2f>& points2,
                    const BoundingBox& bb1,BoundingBox& bb2)    {
  int npoints = (int)points1.size();
  vector<float> xoff(npoints);
  vector<float> yoff(npoints);
  for (int i=0;i<npoints;i++){
      xoff[i]=points2[i].x-points1[i].x;
      yoff[i]=points2[i].y-points1[i].y;
  }
  float dx = median(xoff);
  float dy = median(yoff);

  vector<float> d;
  d.reserve(npoints*(npoints-1)/2);
  for (int i=0;i<npoints;i++){
    for (int j=i+1;j<npoints;j++){
        d.push_back(norm(points2[i]-points2[j])/norm(points1[i]-points1[j]));
    }
}
  float s = median(d);
  bb2.x = round(bb1.x +dx);
  bb2.y = round(bb1.y +dy);
  bb2.width = round(bb1.width*s);
  bb2.height = round(bb1.height*s);
}

bool compare_conf ( const pair<int,float>& idx1,const pair<int,float>& idx2){
  return idx1.second > idx2.second;
}

void TLD::detect(const cv::Mat& frame){
  double t = (double)getTickCount();
  int bbox_step =7;
  Mat img;//TODO:preallocate
  integral(frame,iisum,iisqsum);
  GaussianBlur(frame,img,Size(9,9),1.5);
  int numtrees = classifier.getNumStructs();
  float fern_th = classifier.getFernTh();
  vector<int> ferns(numtrees);
  float conf;
  vector<pair<int,float> > conf_idxs;
  //tmpconf.clear();
  conf_idxs.reserve(grid.size()/bbox_step);
//TODO: blur img
  int a=0;
  for (int i=0;i<grid.size();i+=bbox_step){//FIXME: BottleNeck
      if (getVar(grid[i],iisum,iisqsum)>var){
          classifier.getFeatures(img,grid[i],grid[i].sidx,ferns);
          conf = classifier.measure_forest(ferns);
          if (conf>numtrees*fern_th){
              conf_idxs.push_back(make_pair(i,conf));
              a++;
          }
      }
  }
  if (conf_idxs.size()>100){
      nth_element(conf_idxs.begin(),conf_idxs.begin()+100,conf_idxs.end(),compare_conf);
      conf_idxs.resize(100);
  }
  if (conf_idxs.size()==0){
      detected=false;
      return;
  }
  printf("Fern detector made %d detections ",(int)conf_idxs.size());
  t=(double)getTickCount()-t;
  printf("in %gms\n", t*1000/getTickFrequency());
//TODO!: implement detection structure
//  % initialize detection structure
//  dt.bb     = tld.grid(1:4,idx_dt); % bounding boxes
//  dt.patt   = tld.tmp.patt(:,idx_dt); % corresponding codes of the Ensemble Classifier
//  dt.idx    = find(idx_dt); % indexes of detected bounding boxes within the scanning grid
//  dt.conf1  = nan(1,num_dt); % Relative Similarity (for final nearest neighbour classifier)
//  dt.conf2  = nan(1,num_dt); % Conservative Similarity (for integration with tracker)
//  dt.isin   = nan(3,num_dt); % detected (isin=1) or rejected (isin=0) by nearest neighbour classifier
//  dt.patch  = nan(prod(tld.model.patchsize),num_dt); % Corresopnding patches
  //  for i = 1:num_dt % for every remaining detection
  //      ex   = tldGetPattern(img,dt.bb(:,i),tld.model.patchsize); % measure patch
  //      [conf1, conf2, isin] = tldNN(ex,tld); % evaluate nearest neighbour classifier
  //      % fill detection structure
  //      dt.conf1(i)   = conf1;
  //      dt.conf2(i)   = conf2;
  //      dt.isin(:,i)  = isin;
  //      dt.patch(:,i) = ex;
  //      if tld.PRINT_DEBUG==1
  //          fprintf('Detector [Frame %d]: Testing Feature %d/%d - %f Conf1 cmp %f\n',I,i,num_dt,conf1,tld.model.thr_nn);
  //      end
  //  end
Mat pattern(patch_size,patch_size,CV_32F);
vector<int> isin(3,-1);
a=0;
float conf1,conf2;
float nn_th = classifier.getNNTh();
Scalar mean,stdev;
dbb.clear();
dconf.clear();
  for (int i=0;i<conf_idxs.size();i++){
    getPattern(frame(grid[conf_idxs[i].first]),pattern,mean,stdev);
    classifier.NNConf(pattern,isin,conf1,conf2);
    if (conf1>nn_th){                                              //  idx = dt.conf1 > tld.model.thr_nn; % get all indexes that made it through the nearest neighbour
      dbb.push_back(grid[conf_idxs[i].first]);                     //  BB    = dt.bb(:,idx); % bounding boxes
      dconf.push_back(conf2);                                      //  Conf  = dt.conf2(:,idx); % conservative confidences
      a++;
    }
}
                                                                   //  tld.dt{I} = dt; % save the whole detection structure
  if (a>0){
      printf("Found %d NN matches\n",a);
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
//  function tld = tldLearning(tld,I)
//  bb    = tld.bb(:,I); % current bounding box
//  img   = tld.img{I}; % current image
//  % Check consistency -------------------------------------------------------
  Scalar mean, stdev;
  Mat pattern;
  getPattern(img(lastbox),pattern,mean,stdev);     //  pPatt  = tldGetPattern(img,bb,tld.model.patchsize); % get current patch
  vector<int> isin;
  float dummy, conf;
  classifier.NNConf(pattern,isin,conf,dummy);      //  [pConf1,dummy9,pIsin] = tldNN(pPatt,tld); % measure similarity to model

if (conf<0.5) {                                    //if pConf1 < 0.5,
    printf("Fast change..not training\n");         //  disp('Fast change.');
    lastvalid =0;                                  //  tld.valid(I) = 0;
    return;                                        //  return;
}                                                  //end % too fast change of appearance
if (pow(stdev.val[0],2)>var){                      //if var(pPatt) < tld.var,
      printf("Low variance..not training\n");      //  disp('Low variance.');
      lastvalid=0;                                 //  tld.valid(I) = 0;
      return;                                      //  return;
}                                                  //end % too low variance of the patch
if(isin[2]==1){                                    //if pIsin(3) == 1  return;
    printf("Patch in negative data..not traing");  //  disp('In negative data.');
    lastvalid=0;                                   //  tld.valid(I) = 0;
    return;                                        //  return;
}                                                  //end % patch is in negative data
//  % Update ------------------------------------------------------------------
                                                   //  % generate positive data
for (int i=0;i<grid.size();i++){                   //  overlap  = bb_overlap(bb,tld.grid); % measure overlap of the current bounding box with the bounding boxes on the grid
    grid[i].overlap = bbOverlap(lastbox,grid[i]);
}
good_boxes.clear();
bad_boxes.clear();
getOverlappingBoxes(lastbox,num_closest_update);
generatePositiveData(img,num_warps_update);        //  [pX,pEx] = tldGeneratePositiveData(tld,overlap,img,tld.p_par_update); % generate positive examples from all bounding boxes that are highly overlappipng with current bounding box
                                                   //  pY       = ones(1,size(pX,2)); % labels of the positive patches
                                                   //  % generate negative data

//  idx      = overlap < tld.n_par.overlap & tld.tmp.conf >= 1; % get indexes of negative bounding boxes on the grid (bounding boxes on the grid that are far from current bounding box and which confidence was larger than 0)
//  overlap  = bb_overlap(bb,tld.dt{I}.bb); % measure overlap of the current bounding box with detections
//  nEx      = tld.dt{I}.patch(:,overlap < tld.n_par.overlap); % get negative patches that are far from current bounding box
//  fern(2,[pX tld.tmp.patt(:,idx)],[pY zeros(1,sum(idx))],tld.model.thr_fern,2); % update the Ensemble Classifier (reuses the computation made by detector)
//  tld = tldTrainNN(pEx,nEx,tld); % update nearest neighbour
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
bool bbcomparator ( const BoundingBox& bb1,const BoundingBox& bb2){
  return bb1.overlap > bb2.overlap;
}

void TLD::getOverlappingBoxes(const cv::Rect& box1,int num_closest){
  float max_overlap = 0;
  for (int i=0;i<grid.size();i++){
      if (grid[i].overlap > max_overlap) {
          max_overlap = grid[i].overlap;
          best_box = grid[i];
      }
      if (grid[i].overlap > 0.6){
          good_boxes.push_back(grid[i]);
      }
      else if (grid[i].overlap < bad_overlap){
          bad_boxes.push_back(grid[i]);
      }
  }
  //Get the best num_closest (10) boxes and puts them in good_boxes
  if (good_boxes.size()>num_closest){
    std::nth_element(good_boxes.begin(),good_boxes.begin()+num_closest,good_boxes.end(),bbcomparator);
    good_boxes.resize(num_closest);
  }
  getBBHull();
}

void TLD::getBBHull(){
  int x1=INT_MAX, x2=0;
  int y1=INT_MAX, y2=0;
  for (int i=0;i<good_boxes.size();i++){
      x1=min(good_boxes[i].x,x1);
      y1=min(good_boxes[i].y,y1);
      x2=max(good_boxes[i].x+good_boxes[i].width,x2);
      y2=max(good_boxes[i].y+good_boxes[i].height,y2);
  }
  bbhull.x = x1;
  bbhull.y = y1;
  bbhull.width = x2-x1;
  bbhull.height = y2 -y1;
}

void TLD::bbPoints(vector<cv::Point2f>& points,const BoundingBox& bb,int pts,int margin){
  float stepx = (bb.width-2*margin)/pts;
  float stepy = (bb.height-2*margin)/pts;
  for (int y=bb.y+margin;y<bb.y+bb.height-margin;y+=stepy){
      for (int x=bb.x+margin;x<bb.x+bb.width-margin;x+=stepx){
          points.push_back(Point2f(x,y));
      }
  }
}
bool bbcomp(const BoundingBox& b1,const BoundingBox& b2){
  TLD t;
    if (t.bbOverlap(b1,b2)<0.5)
      return false;
    else
      return true;
}

void TLD::clusterConf(const vector<BoundingBox>& dbb,const vector<float>& dconf,vector<BoundingBox>& cbb,vector<float>& cconf){
  int numbb =dbb.size();
  vector<int> T;
  float space_thr = 0.5;

  switch (numbb){
  case 1:
    cbb=vector<BoundingBox>(1,dbb[0]);
    cconf=vector<float>(1,dconf[0]);
    return;
    break;
  case 2:
    T =vector<int>(2,0);
    if (1-bbOverlap(dbb[0],dbb[1])>space_thr)
      T[1]=1;
    break;
  default:
    T = vector<int>(numbb);
    partition(dbb,T,(*bbcomp));
    break;
  }
  cconf.reserve(T.size());
  cbb.reserve(T.size());
  printf("classes: ");
  for (int i=0;i<T.size();i++){
      printf("%d ",T[i]);
      float cnf=0;
      BoundingBox bx;
      int N=0,mx=0,my=0,mw=0,mh=0;
      for (int j=0;j<T.size();j++){
          if (T[j]==i){
              cnf=cnf+dconf[T[j]];
              mx=mx+dbb[T[j]].x;
              my=my+dbb[T[j]].y;
              mw=mw+dbb[T[j]].width;
              mh=mh+dbb[T[j]].height;
              N++;
          }
      }
      if (N>0){
          cconf.push_back(cnf/N);
          bx.x=round(mx/N);
          bx.y=round(my/N);
          bx.width=round(mw/N);
          bx.height=round(mh/N);
          cbb.push_back(bx);
      }
  }
  printf("/n");
}

