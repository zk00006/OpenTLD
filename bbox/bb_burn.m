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


function img = bb_burn(img, bb_in, width)

if ~exist('width','var')
    width = 0;
end

if isempty(bb_in)
    return;
end

for i = 1:size(bb_in,2)

    bb = bb_in(:,i);
    bb(1) = max([1 bb(1)]);
    bb(2) = max([1 bb(2)]);
    bb(3) = min([size(img,2), bb(3)]);
    bb(4) = min([size(img,1), bb(4)]);

    
    img(bb(2):bb(2)+width,bb(1):bb(3)) = 255;
    img(bb(4)-width:bb(4),bb(1):bb(3)) = 255;
    img(bb(2):bb(4),bb(1):bb(1)+width) = 255;
    img(bb(2):bb(4),bb(3)-width:bb(3)) = 255;
    
end