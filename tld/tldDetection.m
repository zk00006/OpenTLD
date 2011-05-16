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

function [BB Conf tld] = tldDetection(tld,I)
% scanns the image(I) with a sliding window, returns a list of bounding
% boxes and their confidences that match the object description

BB        = [];
Conf      = [];
dt        = struct('bb',[],'idx',[],'conf1',[],'conf2',[],'isin',nan(3,1),'patt',[],'patch',[]);

img  = tld.img{I};

fern(4,img,tld.control.maxbbox,tld.var,tld.tmp.conf,tld.tmp.patt); % evaluates Ensemble Classifier: saves sum of posteriors to 'tld.tmp.conf', saves measured codes to 'tld.tmp.patt', does not considers patches with variance < tmd.var
idx_dt = find(tld.tmp.conf > tld.model.num_trees*tld.model.thr_fern); % get indexes of bounding boxes that passed throu the Ensemble Classifier
if tld.PRINT_DEBUG==1
    fprintf('Detector [Frame %d]: %d Bounding Boxes Found\n',I,length(idx_dt));
end
if length(idx_dt) > 100 % speedup: if there are more than 100 detections, pict 100 of the most confident only
    [dummy8,sIdx] = sort(tld.tmp.conf(idx_dt),'descend');
    idx_dt = idx_dt(sIdx(1:100));
end

num_dt = length(idx_dt); % get the number detected bounding boxes so-far 
if num_dt == 0
    tld.dt{I} = dt;
    if tld.PRINT_DEBUG==1
        fprintf('Detector [Frame %d]: No Bounding Boxes\n',I);
    end
return; 
end % if nothing detected, return

% initialize detection structure
dt.bb     = tld.grid(1:4,idx_dt); % bounding boxes
dt.patt   = tld.tmp.patt(:,idx_dt); % corresponding codes of the Ensemble Classifier
dt.idx    = find(idx_dt); % indexes of detected bounding boxes within the scanning grid
dt.conf1  = nan(1,num_dt); % Relative Similarity (for final nearest neighbour classifier)
dt.conf2  = nan(1,num_dt); % Conservative Similarity (for integration with tracker)
dt.isin   = nan(3,num_dt); % detected (isin=1) or rejected (isin=0) by nearest neighbour classifier
dt.patch  = nan(prod(tld.model.patchsize),num_dt); % Corresopnding patches

for i = 1:num_dt % for every remaining detection
    
    ex   = tldGetPattern(img,dt.bb(:,i),tld.model.patchsize); % measure patch
    [conf1, conf2, isin] = tldNN(ex,tld); % evaluate nearest neighbour classifier
    
    % fill detection structure
    dt.conf1(i)   = conf1;
    dt.conf2(i)   = conf2;
    dt.isin(:,i)  = isin;
    dt.patch(:,i) = ex;
    if tld.PRINT_DEBUG==1
        fprintf('Detector [Frame %d]: Testing Feature %d/%d - %f Conf1 cmp %f\n',I,i,num_dt,conf1,tld.model.thr_nn);
    end
end

idx = dt.conf1 > tld.model.thr_nn; % get all indexes that made it through the nearest neighbour

% output
BB    = dt.bb(:,idx); % bounding boxes
Conf  = dt.conf2(:,idx); % conservative confidences
tld.dt{I} = dt; % save the whole detection structure



