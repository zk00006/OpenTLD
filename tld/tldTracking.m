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

function [BB2 Conf Valid tld] = tldTracking(tld,BB1,I,J)
% Estimates motion of bounding box BB1 from frame I to frame J

% initialize output variables
BB2    = []; % estimated bounding 
Conf   = []; % confidence of prediction
Valid  = 0;  % is the predicted bounding box valid? if yes, learning will take place ...

if isempty(BB1) || ~bb_isdef(BB1)
    if tld.PRINT_DEBUG==1
        fprintf('Detector [%d]: FAIL - No initial Bounding Box\n',I);
    end
    return;
end % exit function if BB1 is not defined

% estimate BB2
xFI    = bb_points(BB1,10,10,5); % generate 10x10 grid of points within BB1 with margin 5 px
if exist('OCTAVE_VERSION','builtin') % The last argument has been added to enable correct tracking in octave. Why this is necessary is an ongoing question. 
    xFJ    = lk(2,tld.img{I}.input,tld.img{J}.input,xFI,xFI,3);
else % preserve original version of the code for Matlab users.
    xFJ    = lk(2,tld.img{I}.input,tld.img{J}.input,xFI,xFI); % track all points by Lucas-Kanade tracker from frame I to frame J, estimate Forward-Backward error, and NCC for each point
end
medFB  = median2(xFJ(3,:)); % get median of Forward-Backward error
medNCC = median2(xFJ(4,:)); % get median for NCC
idxF   = xFJ(3,:) <= medFB & xFJ(4,:)>= medNCC; % get indexes of reliable points
BB2    = bb_predict(BB1,xFI(:,idxF),xFJ(1:2,idxF)); % estimate BB2 using the reliable points only

tld.xFJ = xFJ(:,idxF); % save selected points (only for display purposes)

% detect failures
if ~bb_isdef(BB2)
	  if tld.PRINT_DEBUG==1
          fprintf('Detector [%d]: FAIL - No Prediction Bounding Box\n',I);
      end
    BB2 = [];
return;
end % bounding box out of image

if bb_isout(BB2,tld.imgsize)
	  if tld.PRINT_DEBUG==1
          fprintf('Detector [%d]: FAIL - Bounding Box out of bounds\n',I);
      end
    BB2 = [];
    return;
end % bounding box out of image


if tld.control.maxbbox > 0 && medFB > 10
	  if tld.PRINT_DEBUG==1
          fprintf('Detector [%d]: FAIL - Forward Backward Error %f>10\n',I,medFB);
      end
    BB2 = [];
    return;
end  % too unstable predictions

% estimate confidence and validity
patchJ   = tldGetPattern(tld.img{J},BB2,tld.model.patchsize); % sample patch in current image
[dummy7,Conf] = tldNN(patchJ,tld); % estimate its Conservative Similarity (considering 50% of positive patches only)

% Validity
Valid    = tld.valid(I); % copy validity from previous frame
if Conf > tld.model.thr_nn_valid, Valid = 1; end % tracker is inside the 'core'



