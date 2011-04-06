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

function [BB2 Conf Valid tld] = tldTrack_occlusion(tld,BB1,I,J)
% Estimates motion of bounding box BB1 from frame I to frame J

% parameters
GRID      = 10; % tracker uses 10x10 grid of pixels that are tracked independently
BIG_FB    = 10; % ???

% output variables
BB2    = []; % estimated bounding box in second frame
Conf   = []; % confidence of prediction, used to combine with detector ..
Valid  = 0;  % is the predicted bounding box valid? if yes, learning will take place ...

if isempty(BB1) || ~bb_isdef(BB1), return; end % exit function if BB1 is not defined

% Forward
xFI    = bb_points(BB1,GRID,GRID,5); % generate grid of points in frame I
xFJ    = lk(2,tld.img{I}.input,tld.img{J}.input,xFI,xFI); % track all points by Lucas-Kanade tracker from frame I to frame J

% Occlusion detection (in development) 
xF1   = bb_points(tld.bb(:,1),GRID,GRID,5); % generate grid of points in the first frame
xB1    = lk(2,tld.img{I}.input,tld.img{1}.input,xFI,xFI,2); % track the points in frame I to the first frame, use only 2 levels of pyramid
idxFB  = xB1(3,:) < 2; % indexes of points that have small forward backward error
idxNCC = xB1(4,:) > 0.9; % indexes of points that have high NCC
idxERR = vnormp(xF1(1:2,:)-xB1(1:2,:),2) > 10; % indexes of poiunts that ended far from the correspoinding points in the first frame
idxOCC = idxFB & idxNCC & idxERR; % occluded points are thos which = have high reliability (FB&NCC) and are far from the original points
xFI    = xFI(:,~idxOCC);
xFJ    = xFJ(:,~idxOCC);

medFB  = median2(xFJ(3,:)); % conservatives
medNCC = median2(xFJ(4,:));
idxF   = xFJ(3,:) <= medFB & xFJ(4,:)>= medNCC;
BB2    = bb_predict(BB1,xFI(:,idxF),xFJ(1:2,idxF));

tld.xFJ = xFJ(:,idxF);
% Failure detection


if ~bb_isdef(BB2) || bb_isout(BB2,tld.imgsize)
    BB2 = [];
    return;
end

if tld.control.maxbbox > 0 && medFB > BIG_FB 

    BB2 = [];
    return;

end

% Output
ex       = tldGetPattern(tld.img{J},BB2,tld.model.patchsize);
[~,Conf] = tldNN(ex,tld);
Valid    = tld.valid(I);

if tld.control.maxbbox > 0
if sum(idxOCC)/length(idxOCC) >= .20
    tld.trackerfailure(J) = 1;
    %disp('tracker failure');
    Valid = 0;
    
    % Try to estimate from first frame
     xF2  = lk(2,tld.img{1}.input,tld.img{I}.input,xF1,xF1);
     medFB  = median2(xF2(3,:)); % conservatives
     
     medNCC = median2(xF2(4,:));
     if medFB < 1 & medNCC > 0.9
     idxF   = xF2(3,:) <= medFB & xF2(4,:)>= medNCC;
     BB2    = bb_predict(tld.bb(:,1),xF1(:,idxF),xF2(1:2,idxF));
     end

end
end
% Match error
% resF  = mean(vnormp(xFJ(1:2,idxF)-xBJ(:,idxF),2)); % residual of forward match
% resB  = mean(vnormp(xBI(1:2,idxB)-xFI(:,idxB),2)); % residual of backward match
% %BB_shift = vnormp(BB_shift,2);
% error = abs(resF-resB);

% if ~bb_isin(BB2,tld.imgsize) || error > OCCLUSION
%     Valid = 0;
% end

