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
dir1 = 'debug_coke_update_1\'; files1 = img_dir(dir1);
dir2 = 'debug_coke_update_2\'; files2 = img_dir(dir2);
dir3 = 'debug_coke_update_3\'; files3 = img_dir(dir3);
dirO = 'coke_1-2-3\';         mkdir(dirO);

for i = 1:size(files1)
    
    img1 = imread(files1(i).name);
    img2 = imread(files2(i).name);
    img3 = imread(files2(i).name);
    
    imgO = [img1 img2 img3];
   
    imwrite(imgO,[dirO n2s(i,5) '.jpg'],'jpeg');
    
end