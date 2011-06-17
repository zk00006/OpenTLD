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
  ///Read Parameters
  //model parameters
  min_win = (int)file["min_win"];
  patch_size = (int)file["patch_size"];
  ncc_thesame = (float)file["ncc_thesame"];
  valid = (float)file["valid"];
  num_trees = (int)file["num_trees"];
  num_features = (int)file["num_features"];
  thr_fern = (float)file["thr_fern"];
  thr_nn = (float)file["thr_nn"];
  thr_nn_valid = (float)file["thr_nn_valid"];
  //initial parameters for positive examples
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
}

void TLD::init(const Mat& frame1,const Rect& box){
  //Get Bounding Boxes
  buildGrid(frame1,box);
  printf("Created %d bounding boxes\n",(int)grid.size());
  getOverlappingBoxes(box,num_closest_init);
  printf("Found %d good boxes, %d bad boxes\n",(int)good_boxes.size(),(int)bad_boxes.size());
  printf("Best Box: %d %d %d %d\n",best_box.x,best_box.y,best_box.width,best_box.height);
  getBBHull(good_boxes,bbhull);
  printf("Bounding box hull: %d %d %d %d\n",bbhull.x,bbhull.y,bbhull.width,bbhull.height);
  //Prepare Classifier
  classifier.prepare(num_trees,num_features,scales);
  //Init Generator
  PatchGenerator generator(0,0,noise_init,true,1-scale_init,1+scale_init,-angle_init*CV_PI/180,angle_init*CV_PI/180,-angle_init*CV_PI/180,angle_init*CV_PI/180);
  // Generate positive data
  generatePositiveData(frame1,generator);
  // Generate negative data
  generateNegativeData(frame1,generator);
  //TODO!: Split Negative examples 50/50
  //TODO!: Train Ferns
  //TODO!: Train NN
  classifier.trainFromSingleView();
  //TODO!: Estimate thresholds on validation Set
  classifier.evaluate();
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
void TLD::generatePositiveData(const Mat& frame, const PatchGenerator& patchGenerator){
  Scalar mean;
  Scalar stdev;
  getPattern(frame(best_box),pEx,mean,stdev);
  var =  pow(stdev.val[0],2)*0.5f;
  cout << "var: " << var << endl;
  //Get Fern features on warped patches
  Mat img;
  Mat warped;
  frame.copyTo(img);
  warped = img(bbhull);
  RNG& rng = theRNG();
  Point2f pt(bbhull.x+(bbhull.width-1)*0.5f,bbhull.y+(bbhull.height-1)*0.5f);
  for (int i=0;i<num_warps_init;i++){
     if (i>0)
       patchGenerator(frame,pt,warped,bbhull.size(),rng);
     for (int b=0;b<good_boxes.size();b++){
         classifier.getFeatures(img,good_boxes[b],good_boxes[b].sidx,ferns,1);
     }
  }
}

void TLD::getPattern(const Mat& img, Mat& pattern,Scalar& mean,Scalar& stdev){
  //Output: resized Zero-Mean patch
  resize(img,pattern,Size(patch_size,patch_size));
  meanStdDev(pattern,mean,stdev);
  pattern = pattern-mean.val[0];
}
void TLD::generateNegativeData(const Mat& frame, const PatchGenerator& patchGenerator){
/* Inputs:
 * - Image
 * - bad_boxes (Boxes far from the bounding box)
 * - variance (pEx variance)
 * Outputs
 * - Negative fern features (nX)
 * - Negative NN examples (nEx)
 */
  vector<int> idx;
  idx = index_shuffle(0,bad_boxes.size());//creates a random indexes from 0 to bad_boxes.size()
  //Get Fern Features of the boxes with big variance (calculated using integral images)
  Mat sum, sqsum;
  double boxvar;
  double mean;
  double sqmean;
  integral(frame,sum,sqsum);
  int a=0;
  int num = std::min((int)bad_boxes.size(),(int)bad_patches*100); //limits the size of bad_boxes to try
  printf("negative data generation started.\n");
  for (int j=0;j<num;j++){
      int i = idx[j];
      if (var>0){
          //TODO: Check the variance calculation
          Point br(bad_boxes[i].br());
          Point bl(bad_boxes[i].x,bad_boxes[i].y+bad_boxes[i].height);
          Point tr(bad_boxes[i].x +bad_boxes[i].width,bad_boxes[i].y);
          Point tl(bad_boxes[i].tl());//bad_boxes[i].x,bad_boxes[i].y);
          mean = (sum.at<double>(br)+sum.at<double>(tl)-sum.at<double>(tr)-sum.at<double>(bl))/bad_boxes[i].area();
          sqmean = (sqsum.at<double>(br)+sqsum.at<double>(tl)-sqsum.at<double>(tr)-sqsum.at<double>(bl))/bad_boxes[i].area();
          boxvar = sqmean-mean*mean;
          if (boxvar<var)
            continue;
      }
      classifier.getFeatures(frame,bad_boxes[i],bad_boxes[i].sidx,ferns,0);
      a++;
  }
  printf("Negative examples generated: %d \n",a);
  //Randomly selects 'bad_patches' and get the patterns for NN;
  Mat negExample;
  Scalar dum1, dum2;
  for (int i=0;i<bad_patches;i++){
      getPattern(frame(bad_boxes[idx[i]]),negExample,dum1,dum2);
      nEx.push_back(negExample);
  }
}


void TLD::trainFromSingleView(const Mat& frame1, const Rect& bbox, const vector<KeyPoint>& kpts, vector<Point2f>& pts){
  //NOTE: Implement training from single frame
  /*Input:
   * -Image
   * -Bounding Box
   * -KeyPoints
   * Output
   * -Trained Fern classifier
   * -Trained Nearest Neightbors classifier
   */
  //Split into good keypoints (within the bounding box) and bad keypoints (far from the bounding box)
  vector<KeyPoint> good_pts;
  vector<KeyPoint> bad_pts;
  splitKeyPoints(kpts,bbox,good_pts,bad_pts);
  KeyPoint::convert(good_pts,pts);
  //Init Classifier
  classifier.trainFromSingleView();

}

void TLD::track(const Mat& img1, const Mat& img2,const vector<Point2f> &points1, vector<Point2f> &points2){
  //Frame-to-frame tracking with forward-backward error cheking
  //TODO: Get confidence of tracked points
  tracker.trackf2f(img1,img2,points1,points2);
}

void TLD::detect(const cv::Mat& frame,vector<KeyPoint>& points, vector<float>& confidence){
  //TODO: implement detection
//  function [BB Conf tld] = tldDetection(tld,I)
//  % scanns the image(I) with a sliding window, returns a list of bounding
//  % boxes and their confidences that match the object description
//
//  BB        = [];
//  Conf      = [];
//  dt        = struct('bb',[],'idx',[],'conf1',[],'conf2',[],'isin',nan(3,1),'patt',[],'patch',[]);
//
//  img  = tld.img{I};
//
//  fern(4,img,tld.control.maxbbox,tld.var,tld.tmp.conf,tld.tmp.patt); % evaluates Ensemble Classifier: saves sum of posteriors to 'tld.tmp.conf', saves measured codes to 'tld.tmp.patt', does not considers patches with variance < tmd.var
//  idx_dt = find(tld.tmp.conf > tld.model.num_trees*tld.model.thr_fern); % get indexes of bounding boxes that passed throu the Ensemble Classifier
//  if tld.PRINT_DEBUG==1
//      fprintf('Detector [Frame %d]: %d Bounding Boxes Found\n',I,length(idx_dt));
//  end
//  if length(idx_dt) > 100 % speedup: if there are more than 100 detections, pict 100 of the most confident only
//      [dummy8,sIdx] = sort(tld.tmp.conf(idx_dt),'descend');
//      idx_dt = idx_dt(sIdx(1:100));
//  end
//
//  num_dt = length(idx_dt); % get the number detected bounding boxes so-far
//  if num_dt == 0
//      tld.dt{I} = dt;
//      if tld.PRINT_DEBUG==1
//          fprintf('Detector [Frame %d]: No Bounding Boxes\n',I);
//      end
//  return;
//  end % if nothing detected, return
//
//  % initialize detection structure
//  dt.bb     = tld.grid(1:4,idx_dt); % bounding boxes
//  dt.patt   = tld.tmp.patt(:,idx_dt); % corresponding codes of the Ensemble Classifier
//  dt.idx    = find(idx_dt); % indexes of detected bounding boxes within the scanning grid
//  dt.conf1  = nan(1,num_dt); % Relative Similarity (for final nearest neighbour classifier)
//  dt.conf2  = nan(1,num_dt); % Conservative Similarity (for integration with tracker)
//  dt.isin   = nan(3,num_dt); % detected (isin=1) or rejected (isin=0) by nearest neighbour classifier
//  dt.patch  = nan(prod(tld.model.patchsize),num_dt); % Corresopnding patches
//
//  for i = 1:num_dt % for every remaining detection
//
//      ex   = tldGetPattern(img,dt.bb(:,i),tld.model.patchsize); % measure patch
//      [conf1, conf2, isin] = tldNN(ex,tld); % evaluate nearest neighbour classifier
//
//      % fill detection structure
//      dt.conf1(i)   = conf1;
//      dt.conf2(i)   = conf2;
//      dt.isin(:,i)  = isin;
//      dt.patch(:,i) = ex;
//      if tld.PRINT_DEBUG==1
//          fprintf('Detector [Frame %d]: Testing Feature %d/%d - %f Conf1 cmp %f\n',I,i,num_dt,conf1,tld.model.thr_nn);
//      end
//  end
//
//  idx = dt.conf1 > tld.model.thr_nn; % get all indexes that made it through the nearest neighbour
//
//  % output
//  BB    = dt.bb(:,idx); % bounding boxes
//  Conf  = dt.conf2(:,idx); % conservative confidences
//  tld.dt{I} = dt; % save the whole detection structure
}

void TLD::evaluate(){
  //TODO: implement evaluation
//  % INTEGRATOR --------------------------------------------------------------
//    DT = 1; if isempty(dBB), DT = 0; end % is detector defined?
//    TR = 1; if isempty(tBB), TR = 0; end % is tracker defined?
//    if TR % if tracker is defined
//        % copy tracker's result
//        tld.bb(:,I)  = tBB;
//        tld.conf(I)  = tConf;
//        tld.size(I)  = 1;
//        tld.valid(I) = tValid;
//        if tld.PRINT_DEBUG==1
//            fprintf('Status [%d]: Tracking is occuring\n',I);
//        end
//        if DT % if detections are also defined
//            if tld.PRINT_DEBUG==1
//                fprintf('Status [%d]: Detection is occuring\n',I);
//            end
//            [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
//            id = bb_overlap(tld.bb(:,I),cBB) < 0.5 & cConf > tld.conf(I); % get indexes of all clusters that are far from tracker and are more confident then the tracker
//            if sum(id) == 1 % if there is ONE such a cluster, re-initialize the tracker
//                if tld.PRINT_DEBUG==1
//                    fprintf('Status [%d]: Re-Init BB\n',I);
//                end
//                tld.bb(:,I)  = cBB(:,id);
//                tld.conf(I)  = cConf(:,id);
//                tld.size(I)  = cSize(:,id);
//                tld.valid(I) = 0;
//            else % othervide adjust the tracker's trajectory
//                if tld.PRINT_DEBUG==1
//                    fprintf('Status [%d]: Average BB\n',I);
//                end
//                idTr = bb_overlap(tBB,tld.dt{I}.bb) > 0.7;  % get indexes of close detections
//                tld.bb(:,I) = mean([repmat(tBB,1,10) tld.dt{I}.bb(:,idTr)],2);  % weighted average trackers trajectory with the close detections
//
//            end
//        end
//    else % if tracker is not defined
//        if DT % and detector is defined
//            if tld.PRINT_DEBUG==1
//                fprintf('Status [%d]: Detection is occuring\n',I);
//            end
//            [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
//            if length(cConf) == 1 % and if there is just a single cluster, re-initalize the tracker
//                if tld.PRINT_DEBUG==1
//                    fprintf('Status [%d]: Re-Init BB\n',I);
//                end
//                tld.bb(:,I)  = cBB;
//                tld.conf(I)  = cConf;
//                tld.size(I)  = cSize;
//                tld.valid(I) = 0;
//            end
//        end
//    end
}

void TLD::learn(){
  //TODO: implement learning
//  function tld = tldLearning(tld,I)
//  bb    = tld.bb(:,I); % current bounding box
//  img   = tld.img{I}; % current image
//  % Check consistency -------------------------------------------------------
//  pPatt  = tldGetPattern(img,bb,tld.model.patchsize); % get current patch
//  [pConf1,dummy9,pIsin] = tldNN(pPatt,tld); % measure similarity to model
//  if pConf1 < 0.5, disp('Fast change.'); tld.valid(I) = 0; return; end % too fast change of appearance
//  if var(pPatt) < tld.var, disp('Low variance.'); tld.valid(I) = 0; return; end % too low variance of the patch
//  if pIsin(3) == 1, disp('In negative data.'); tld.valid(I) = 0; return; end % patch is in negative data
//  % Update ------------------------------------------------------------------
//  % generate positive data
//  overlap  = bb_overlap(bb,tld.grid); % measure overlap of the current bounding box with the bounding boxes on the grid
//  [pX,pEx] = tldGeneratePositiveData(tld,overlap,img,tld.p_par_update); % generate positive examples from all bounding boxes that are highly overlappipng with current bounding box
//  pY       = ones(1,size(pX,2)); % labels of the positive patches
//  % generate negative data
//  idx      = overlap < tld.n_par.overlap & tld.tmp.conf >= 1; % get indexes of negative bounding boxes on the grid (bounding boxes on the grid that are far from current bounding box and which confidence was larger than 0)
//  overlap  = bb_overlap(bb,tld.dt{I}.bb); % measure overlap of the current bounding box with detections
//  nEx      = tld.dt{I}.patch(:,overlap < tld.n_par.overlap); % get negative patches that are far from current bounding box
//  fern(2,[pX tld.tmp.patt(:,idx)],[pY zeros(1,sum(idx))],tld.model.thr_fern,2); % update the Ensemble Classifier (reuses the computation made by detector)
//  tld = tldTrainNN(pEx,nEx,tld); % update nearest neighbour
}

void TLD::buildGrid(const cv::Mat& img, const cv::Rect& box){
  //TODO: Set shift and scales dinamically
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
      if (grid[i].overlap > 0.6){ //TODO: Read from parameters file
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
}

void TLD::getBBHull(const vector<BoundingBox>& boxes,BoundingBox& bbhull){
  int x1=INT_MAX, x2=0;
  int y1=INT_MAX, y2=0;
  for (int i=0;i<boxes.size();i++){
      x1=min(boxes[i].x,x1);
      y1=min(boxes[i].y,y1);
      x2=max(boxes[i].x+boxes[i].width,x2);
      y2=max(boxes[i].y+boxes[i].height,y2);
  }
  bbhull.x = x1;
  bbhull.y = y1;
  bbhull.width = x2-x1;
  bbhull.height = y2 -y1;
}

void TLD::splitKeyPoints(const vector<KeyPoint>& kpts,const Rect& bbox,vector<KeyPoint>& good_pts,vector<KeyPoint>& bad_pts){
  for (int i=0;i<kpts.size();i++){
      if (kpts[i].pt.x < bbox.x || kpts[i].pt.y<bbox.y ||kpts[i].pt.x>bbox.x+bbox.width || kpts[i].pt.y>bbox.y+bbox.height)
        bad_pts.push_back(kpts[i]);
      else
        good_pts.push_back(kpts[i]);
  }
}
