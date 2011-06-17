/*
 * FerNNClassifier.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: alantrrs
 */

#include <FerNNClassifier.h>

using namespace cv;

void FerNNClassifier::prepare(int num_trees, int num_features, const vector<Size>& scales){
  //Initialize test locations for features
  nstructs = num_trees;
  structSize = num_features;
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
  //Initialize
}

void FerNNClassifier::getFeatures(const cv::Mat& image,const cv::Rect& box, int scale_idx){
  vector<int> fern(nstructs);
  int leaf;
  for (int t=0;t<nstructs;t++){
      leaf=0;
      for (int f=0; f<structSize; f++){
          leaf = (leaf << 1) + features[scale_idx][t*nstructs+f](image(box));
      }
      fern[t]=leaf;
  }
  ferns.push_back(fern);
}

void FerNNClassifier::trainFromSingleView(){

}


void FerNNClassifier::evaluate(){

}

void FerNNClassifier::update(){

}

//  NOTE: tldNN
//  function [conf1,conf2,isin] = tldNN(x,tld)
//  % 'conf1' ... full model (Relative Similarity)
//  % 'conf2' ... validated part of model (Conservative Similarity)
//  % 'isnin' ... inside positive ball, id positive ball, inside negative ball
//
//  isin = nan(3,size(x,2));
//
//  if isempty(tld.pex) % IF positive examples in the model are not defined THEN everything is negative
//      conf1 = zeros(1,size(x,2));
//      conf2 = zeros(1,size(x,2));
//      return;
//  end
//
//  if isempty(tld.nex) % IF negative examples in the model are not defined THEN everything is positive
//      conf1 = ones(1,size(x,2));
//      conf2 = ones(1,size(x,2));
//      return;
//  end
//
//  conf1 = nan(1,size(x,2));
//  conf2 = nan(1,size(x,2));
//
//  for i = 1:size(x,2) % fore every patch that is tested
//
//      nccP = distance(x(:,i),tld.pex,1); % measure NCC to positive examples
//      nccN = distance(x(:,i),tld.nex,1); % measure NCC to negative examples
//
//      % set isin
//      if any(nccP > tld.model.ncc_thesame), isin(1,i) = 1;  end % IF the query patch is highly correlated with any positive patch in the model THEN it is considered to be one of them
//      [dummy6,isin(2,i)] = max(nccP); % get the index of the maximall correlated positive patch
//      if any(nccN > tld.model.ncc_thesame), isin(3,i) = 1;  end % IF the query patch is highly correlated with any negative patch in the model THEN it is considered to be one of them
//
//      % measure Relative Similarity
//      dN = 1 - max(nccN);
//      dP = 1 - max(nccP);
//      conf1(i) = dN / (dN + dP);
//
//      % measure Conservative Similarity
//      maxP = max(nccP(1:ceil(tld.model.valid*size(tld.pex,2))));
//      dP = 1 - maxP;
//      conf2(i) = dN / (dN + dP);
//
//  end
