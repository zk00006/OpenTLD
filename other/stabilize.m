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

clc;
images = 'D:\_TLD_Video\bety\5272\';
% images = 'D:\PhD\Data\TLD\07_motocross\';

files = img_dir(images);
% bb = dlmread([images 'tld.txt'])';
BB = dlmread([images 'tld.txt'])';
CP = bb_center(BB);
cpold = nan;
for i = 1:length(files)
    
    img1 = imread(files(i).name);
    
    
    cp = CP(:,i);
    
    if ~isnan(cp(1))
        
        H = size(img1,1)/2;
        W = size(img1,2)/2;
        bb = [cp(1)-W/2+1, cp(2)-H/2+1, cp(1)+W/2, cp(2)+H/2]';
        %bb = bb_rescale_relative(BB(:,i),[2 2]);
        img2 = img_patch(img1,bb);
    else
        img2(:,:,:) = 0;
    end
   
    is = size(img2);
    IMG = [imresize(img1,[180 320]) imresize(img2,[180 320])];
    
    imshow(IMG);
    
    drawnow;
    end