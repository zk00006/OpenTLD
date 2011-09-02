/*
 * FerNNClassifier.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: alantrrs
 */

#include <FerNNClassifier.h>
#include <stdio.h>
using namespace cv;

void FerNNClassifier::read(const FileNode& file){
  ///Classifier Parameters
  valid = (float)file["valid"];
  ncc_thesame = (float)file["ncc_thesame"];
  nstructs = (int)file["num_trees"];
  structSize = (int)file["num_features"];
  thr_fern = (float)file["thr_fern"];
  thr_nn = (float)file["thr_nn"];
  thr_nn_valid = (float)file["thr_nn_valid"];
}

void FerNNClassifier::prepare(const vector<Size>& scales){
  //Initialize test locations for features
  int totalFeatures = nstructs*structSize;
  features = vector<vector<Feature> >(scales.size(),vector<Feature> (totalFeatures));
  RNG& rng = theRNG();
  float x1f,x2f,y1f,y2f;
  int x1, x2, y1, y2;
  for (int i=0;i<totalFeatures;i++){
      x1f = (float)rng;
      y1f = (float)rng;
      x2f = (float)rng;
      y2f = (float)rng;
      for (int s=0;s<scales.size();s++){
          x1 = x1f * scales[s].width;
          y1 = y1f * scales[s].height;
          x2 = x2f * scales[s].width;
          y2 = y2f * scales[s].height;
          features[s][i] = Feature(x1, y1, x2, y2);
      }

  }
  //Thresholds
  thrN = 0.5*nstructs;

  //Initialize Posteriors
  for (int i = 0; i<nstructs; i++) {
      posteriors.push_back(vector<float>(pow(2.0,structSize), 0));
      pCounter.push_back(vector<int>(pow(2.0,structSize), 0));
      nCounter.push_back(vector<int>(pow(2.0,structSize), 0));
  }
}

void FerNNClassifier::getFeatures(const cv::Mat& image,const cv::Rect& box,const int& scale_idx, vector<int>& fern){
  int leaf;
  for (int t=0;t<nstructs;t++){
      leaf=0;
      for (int f=0; f<structSize; f++){
          leaf = (leaf << 1) + features[scale_idx][t*nstructs+f](image(box));
      }
      fern[t]=leaf;
  }
}

float FerNNClassifier::measure_forest(vector<int> fern) {
  float votes = 0;
  for (int i = 0; i < nstructs; i++) {
      votes += posteriors[i][fern[i]];
  }
  return votes;
}

void FerNNClassifier::update(const vector<int>& fern, int C, int N) {
  for (int i = 0; i < nstructs; i++) {
      (C==1) ? pCounter[i][fern[i]] += N : nCounter[i][fern[i]] += N;
      if (pCounter[i][fern[i]]==0) {
          posteriors[i][fern[i]] = 0;
      } else {
          posteriors[i][fern[i]] = ((float)(pCounter[i][fern[i]]))/(pCounter[i][fern[i]] + nCounter[i][fern[i]]);
      }
  }
}

void FerNNClassifier::trainF(const vector<pair<vector<int>,int> >& ferns,int resample){
  // Conf = function(2,X,Y,Margin,Bootstrap,Idx)
  //                 0 1 2 3      4         5
  //  double *X     = mxGetPr(prhs[1]); -> ferns[i].first
  //  int numX      = mxGetN(prhs[1]);  -> ferns.size()
  //  double *Y     = mxGetPr(prhs[2]); ->ferns[i].second
  //  double thrP   = *mxGetPr(prhs[3]) * nTREES; ->threshold*nstructs
  //  int bootstrap = (int) *mxGetPr(prhs[4]); ->resample
  thrP = thr_fern*nstructs;                                                          // int step = numX / 10;
  for (int j = 0; j < resample; j++) {                      // for (int j = 0; j < bootstrap; j++) {
      for (int i = 0; i < ferns.size(); i++){               //   for (int i = 0; i < step; i++) {
                                                            //     for (int k = 0; k < 10; k++) {
                                                            //       int I = k*step + i;//box index
                                                            //       double *x = X+nTREES*I; //tree index
          if(ferns[i].second==1){                           //       if (Y[I] == 1) {
              if(measure_forest(ferns[i].first)<=thrP)      //         if (measure_forest(x) <= thrP)
                update(ferns[i].first,1,1);                 //             update(x,1,1);
          }else{                                            //        }else{
              if (measure_forest(ferns[i].first) >= thrN)   //         if (measure_forest(x) >= thrN)
                update(ferns[i].first,0,1);                 //             update(x,0,1);
          }
      }
  }
}

void FerNNClassifier::trainNN(const vector<cv::Mat>& nn_examples){
  //  function tld = tldTrainNN(pEx,nEx,tld)
  //  nP = size(pEx,2); % get the number of positive example
  //  nN = size(nEx,2); % get the number of negative examples
  //  x = [pEx,nEx];
  //  y = [ones(1,nP), zeros(1,nN)];
  //  % Permutate the order of examples
  //  idx = randperm(nP+nN); %
  //  if ~isempty(pEx)
  //      x   = [pEx(:,1) x(:,idx)]; % always add the first positive patch as the first (important in initialization)
  //      y   = [1 y(:,idx)];
  //  end
  float conf,dummy;
  vector<int> y(nn_examples.size(),0);
  y[0]=1;
  vector<int> isin;
  for (int i=0;i<nn_examples.size();i++){                          //  for i = 1:length(y)
      NNConf(nn_examples[i],isin,conf,dummy);                      //    [conf1,dummy5,isin] = tldNN(x(:,i),tld); % measure Relative similarity
      //printf("conf: %f pEx:%d  nEx:%d\n",conf,(int)pEx.size(),(int)nEx.size());
      if (y[i]==1 && conf<=thr_nn){                                //    if y(i) == 1 && conf1 <= tld.model.thr_nn % 0.65
          if (isin[1]<0){                                          //      if isnan(isin(2))
              pEx = vector<Mat>(1,nn_examples[i]);                 //        tld.pex = x(:,i);
              continue;                                            //        continue;
          }                                                        //      end
          pEx.insert(pEx.begin()+isin[1],nn_examples[i]);          //      tld.pex = [tld.pex(:,1:isin(2)) x(:,i) tld.pex(:,isin(2)+1:end)]; % add to model
      }                                                            //    end
      if(y[i]==0 && conf>0.5)                                      //  if y(i) == 0 && conf1 > 0.5
        nEx.push_back(nn_examples[i]);                             //    tld.nex = [tld.nex x(:,i)];

  }                                                                 //  end
  printf("Trained NN examples: %d positive %d negative\n",(int)pEx.size(),(int)nEx.size());
}                                                                  //  end


void FerNNClassifier::NNConf(const Mat& example, vector<int>& isin,float& rsconf,float& csconf){
  /*Inputs:
   * -NN Patch
   * Outputs:
   * -Relative Similarity (rsconf), Conservative Similarity (csconf), In pos. set|Id pos set|In neg. set (isin)
   */
  isin=vector<int>(3,-1);//TODO: replace with static array     isin = nan(3,size(x,2)); -
  if (pEx.empty()){ //if isempty(tld.pex) % IF positive examples in the model are not defined THEN everything is negative
      rsconf = 0; //    conf1 = zeros(1,size(x,2));
      csconf=0;
      return;
  }
  if (nEx.empty()){ //if isempty(tld.nex) % IF negative examples in the model are not defined THEN everything is positive
      rsconf = 1;   //    conf1 = ones(1,size(x,2));
      csconf=1;
      return;
  }
  //for i = 1:size(x,2) % fore every patch that is tested
  Mat ncc(1,1,CV_32F);
  float nccP,csmaxP,maxP=0;
  bool anyP=false;
  int maxPidx,validatedPart = ceil(pEx.size()*valid);
  float nccN, maxN=0;
  bool anyN=false;
  for (int i=0;i<pEx.size();i++){
      matchTemplate(pEx[i],example,ncc,CV_TM_CCORR_NORMED);//    nccP = distance(x(:,i),tld.pex,1); % measure NCC to positive examples
      nccP=(((float*)ncc.data)[0]+1)*0.5;
      if (nccP>ncc_thesame)
        anyP=true;
      if(nccP > maxP){
          maxP=nccP;
          maxPidx = i;
          if(i<validatedPart)
            csmaxP=maxP; //maxP = max(nccP(1:ceil(tld.model.valid*size(tld.pex,2))));
      }
     // printf("nccp=%f ",nccP);
  }
  for (int i=0;i<nEx.size();i++){
      matchTemplate(nEx[i],example,ncc,CV_TM_CCORR_NORMED);//    nccN = distance(x(:,i),tld.nex,1); % measure NCC to negative examples
      nccN=(((float*)ncc.data)[0]+1)*0.5;
      if (nccN>ncc_thesame)
        anyN=true;
      if(nccN > maxN)
        maxN=nccN;
     // printf("nccn=%f ",nccN);
  }
  //set isin
  if (anyP) isin[0]=1;//    if any(nccP > tld.model.ncc_thesame), isin(1,i) = 1;  end % IF the query patch is highly correlated with any positive patch in the model THEN it is considered to be one of them
  isin[1]=maxPidx;//    [dummy6,isin(2,i)] = max(nccP); % get the index of the maximall correlated positive patch
  if (anyN) isin[2]=1;//    if any(nccN > tld.model.ncc_thesame), isin(3,i) = 1;  end % IF the query patch is highly correlated with any negative patch in the model THEN it is considered to be one of them
  //Measure Relative Similarity
  float dN=1-maxN;//    dN = 1 - max(nccN);
  float dP=1-maxP;//    dP = 1 - max(nccP);
  rsconf = (float)dN/(dN+dP);//    conf1(i) = dN / (dN + dP);
  //Measure Conservative Similarity
  dP = 1 - csmaxP;
  csconf =(float)dN / (dN + dP);
}

void FerNNClassifier::evaluateTh(const vector<pair<vector<int>,int> >& nXT,const vector<cv::Mat>& nExT){
float fconf;
  for (int i=0;i<nXT.size();i++){
    fconf = (float) measure_forest(nXT[i].first)/nstructs;
    if (fconf>thr_fern)
      thr_fern=fconf;
}
  vector <int> isin;
  float conf,dummy;
  for (int i=0;i<nExT.size();i++){
      NNConf(nExT[i],isin,conf,dummy);
      if (conf>thr_nn)
        thr_nn=conf;
  }
  if (thr_nn>thr_nn_valid)
    thr_nn_valid = thr_nn;
}

