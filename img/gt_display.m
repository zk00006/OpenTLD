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


path = 'X:\PhD\Publications\2009-06_OLCV2009\Web\MultiFace\';
clf;
files = img_dir(path);
load([path 'gt.mat']);
for i = 1:length(files)
    img =  imread([path files(i).name]);
    for ii = 1:length(gt)
        img = bb_burn(img,gt{ii}.bb(:,1763+i),255);
    end
    imshow(img);
    drawnow;
end