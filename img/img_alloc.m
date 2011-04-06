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


function img = img_alloc(in,imsize)
% Allocates image structure. 

if ischar(in)
    in = imread(in);
end

if ndims(in) == 3
    %img.input = in(:,:,2);
    img.input = rgb2gray(in);
    %img.input = imadjust(img.input);
else
    img.input = in;
end

% img.input = fliplr(img.input);

if exist('imsize','var')
    img.input = imresize(img.input,imsize);
end

 img.blur = img_blur(img.input,2);
%img.blur = img_blur(img.input,5);

