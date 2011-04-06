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

% -------------------------------------------------------------------------

% Initialization of camera
% Try to run independently in Matlab (requires Image Acquisition toolbox)
% if it does not work, modify the first two lines

%source.vid = videoinput('winvideo', 1);
%source.vid = videoinput('winvideo', 1,'RGB24_320x240');
source.vid = videoinput('winvideo', 1,'YUY2_320x240');

set(source.vid,'ReturnedColorSpace','grayscale');
vidRes = get(source.vid, 'VideoResolution');
nBands = get(source.vid, 'NumberOfBands');
hImage = image( zeros(vidRes(2), vidRes(1), nBands) );

% open a figure that shows the raw video stream
preview(source.vid, hImage);
% makes the raw stream invisible
set(1,'visible','off');