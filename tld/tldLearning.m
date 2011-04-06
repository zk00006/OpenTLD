% Copyright 2011 Zdenek Kalal
%
% This file is part of TLD.
% 
% TLD is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% TLD is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with TLD.  If not, see <http://www.gnu.org/licenses/>.

function tld = tldLearning(tld,I)

bb    = tld.bb(:,I); % current bounding box
img   = tld.img{I}; % current image

% Check consistency -------------------------------------------------------

pPatt  = tldGetPattern(img,bb,tld.model.patchsize); % get current patch
[pConf1,~,pIsin] = tldNN(pPatt,tld); % measure similarity to model

if pConf1 < 0.5, disp('Fast change.'); tld.valid(I) = 0; return; end % too fast change of appearance
if var(pPatt) < tld.var, disp('Low variance.'); tld.valid(I) = 0; return; end % too low variance of the patch
if pIsin(3) == 1, disp('In negative data.'); tld.valid(I) = 0; return; end % patch is in negative data

% Update ------------------------------------------------------------------

% generate positive data
overlap  = bb_overlap(bb,tld.grid); % measure overlap of the current bounding box with the bounding boxes on the grid
[pX,pEx] = tldGeneratePositiveData(tld,overlap,img,tld.p_par_update); % generate positive examples from all bounding boxes that are highly overlappipng with current bounding box
pY       = ones(1,size(pX,2)); % labels of the positive patches

% generate negative data
idx      = overlap < tld.n_par.overlap & tld.tmp.conf >= 1; % get indexes of negative bounding boxes on the grid (bounding boxes on the grid that are far from current bounding box and which confidence was larger than 0)
overlap  = bb_overlap(bb,tld.dt{I}.bb); % measure overlap of the current bounding box with detections
nEx      = tld.dt{I}.patch(:,overlap < tld.n_par.overlap); % get negative patches that are far from current bounding box

fern(2,[pX tld.tmp.patt(:,idx)],[pY zeros(1,sum(idx))],tld.model.thr_fern,2); % update the Ensemble Classifier (reuses the computation made by detector)
tld = tldTrainNN(pEx,nEx,tld); % update nearest neighbour 


