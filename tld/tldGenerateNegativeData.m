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

function [nX,nEx] = tldGenerateNegativeData(tld,overlap,img)

% Measure patterns on all bboxes that are far from initial bbox

idxN        = find(overlap<tld.n_par.overlap);
[nX,status] = fern(5,img,idxN,tld.var/2);
idxN        = idxN(status==1); % bboxes far and with big variance
nX          = nX(:,status==1);

% Randomly select 'num_patches' bboxes and measure patches
idx = randvalues(1:length(idxN),tld.n_par.num_patches);
bb  = tld.grid(:,idxN(idx));
nEx = tldGetPattern(img,bb,tld.model.patchsize);