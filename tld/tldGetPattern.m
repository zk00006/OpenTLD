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

function pattern = tldGetPattern(img,bb,patchsize,flip)
% get patch under bounding box (bb), normalize it size, reshape to a column
% vector and normalize to zero mean and unit variance (ZMUV)

% initialize output variable
nBB = size(bb,2);
pattern = zeros(prod(patchsize),nBB);
if ~exist('flip','var')
    flip= 0;
end

% for every bounding box
for i = 1:nBB
    
    % sample patch
    patch = img_patch(img.input,bb(:,i));
    
    % flip if needed
    if flip
        patch = fliplr(patch);
    end
    
    % normalize size to 'patchsize' and nomalize intensities to ZMUV
    pattern(:,i) = tldPatch2Pattern(patch,patchsize);
end
