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

function tld = tldInitDetector(tld)

fern(0);
fern(1,tld.source.im0.input,tld.grid,tld.features,tld.scales);

% Variance
tld.pex    = [tld.pex tldGetPattern(tld.source.im0,tld.source.bb,tld.patchsize)];
tld.pex_conf  = 1;
tld.var    = [var(tld.pex)/2, var(tld.pex)*2];

% Fern
overlap = bb_overlap(tld.source.bb,tld.grid);

[pX,pIdx] = tldGeneratePositiveData(tld,overlap,tld.img{1},300);
pY = ones(1,size(pX,2));

[nX,nIdx] = tldGenerateNegativeData(tld,overlap,tld.img{1});
nY   = zeros(1,size(nX,2));

X = [pX nX];
Y = [pY nY];
IDX = [pIdx nIdx];
idx = randperm(size(X,2));
X = X(:,idx);
Y = Y(:,idx);
IDX = IDX(:,idx);

conf = fern(2,X,Y,1,2);

% Nearest-Neighbour
idx = Y==0 & conf >= tld.features.margin-2;
if sum(idx)==0, return; end;

nex = tldGetPattern(tld.source.im0,tld.grid(1:4,IDX(idx)),tld.patchsize);
ncc = distance(tld.pex,nex,1);

num_nex = min([length(ncc) 10]);

[sNcc sIdx] = sort(ncc,'descend');

tld.ncc = max(sNcc(1),tld.ncc);
tld.nex = nex(:,sIdx(1:num_nex));
