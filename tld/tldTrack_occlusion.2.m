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

GRID      = tld.tracker.grid;
BIG_FB    = tld.tracker.big_fb;
% OCCLUSION = tld.tracker.occlusion;

BB2    = []; 
Conf   = []; 
Valid  = 0;

if isempty(BB1) || ~bb_isdef(BB1),
    return;
end

% Forward
xFI    = bb_points(BB1,GRID,GRID,5);
xFJ    = lk(2,tld.img{I}.input,tld.img{J}.input,xFI,xFI,3);
idxFB  = xFJ(3,:) < 2;
idxNCC = xFJ(4,:) > 0.9;

% Occlusion detection
 xF1    = bb_points(tld.bb(:,1),GRID,GRID,5);
 xB1    = lk(2,tld.img{I}.input,tld.img{1}.input,xFI,xFI,1);
 idxOCC = xB1(3,:) < .1 & xB1(4,:) > 0.95 & vnormp(xF1(1:2,:)-xB1(1:2,:),2) > 10;

xFI    = xFI(:,~idxOCC & idxFB & idxNCC);
xFJ    = xFJ(:,~idxOCC & idxFB & idxNCC);

medFB  = median2(xFJ(3,:));
medNCC = median2(xFJ(4,:));
idxF   = xFJ(3,:) <= medFB & xFJ(4,:)>= medNCC;
BB2    = bb_predict(BB1,xFI(:,idxF),xFJ(1:2,idxF));

% Failure detection
if medFB > BIG_FB || ~bb_isdef(BB2) || bb_isout(BB2,tld.imgsize)
    disp('Tracker uncertain.');
    BB2 = [];
    return;
end

% Output
ex       = tldGetPattern(tld.img{J},BB2,tld.model.patchsize);
[~,Conf] = tldNN(ex,tld);
Valid    = tld.valid(I);

% if sum(idxOCC)/length(idxOCC) >= .20
%     tld.trackerfailure(J) = 1;
%     disp('Tracker occlusion.');
%     %disp('tracker failure');
%     Valid = 0;
% end

% Match error
% resF  = mean(vnormp(xFJ(1:2,idxF)-xBJ(:,idxF),2)); % residual of forward match
% resB  = mean(vnormp(xBI(1:2,idxB)-xFI(:,idxB),2)); % residual of backward match
% %BB_shift = vnormp(BB_shift,2);
% error = abs(resF-resB);

% if ~bb_isin(BB2,tld.imgsize) || error > OCCLUSION
%     Valid = 0;
% end

