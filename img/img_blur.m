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

function M = img_blur(M,sigma)
% Blures gray scale image with gaussian kernel. 

if nargin==1
    sigma = 1.5;
end

csize = 6*sigma;

shift = (csize - 1)/2;

h = fspecial('gaussian',csize,sigma);

M = conv2(M,h);
M = M(1+shift:end-shift,1+shift:end-shift);
M = uint8(M);
