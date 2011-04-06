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

function [BB2 Conf Valid] = tldTrack(tld,BB1,I,J)

GRID   = 10;
BIG_FB = 10;

BB2    = []; 
Conf   = []; 
Valid  = 0;

if isempty(BB1) || ~bb_isdef(BB1),
    return;
end

% Tracking
ptI    = bb_points(BB1,GRID,GRID,5);
ptJ    = lk(2,tld.img{I}.input,tld.img{J}.input,ptI,ptI);
medNCC = median2(ptJ(4,:));
idx = ptJ(4,:) > medNCC;
medFB  = median2(ptJ(3,idx));

idx    = ptJ(3,:) < medFB & ptJ(4,:)> medNCC;
BB     = bb_predict(BB1,ptI(:,idx),ptJ(1:2,idx));

% Failure detection
if medFB > BIG_FB || ~bb_isdef(BB) || bb_isout(BB,tld.imgsize)
    return;
end

% Output
BB2   = BB;
ex    = tldGetPattern(tld.img{J},BB2,tld.model.patchsize);
Conf  = tldNN(ex,tld);
Valid = tld.valid(I);
if ~bb_isin(BB,tld.imgsize)
    Valid = 0;
end