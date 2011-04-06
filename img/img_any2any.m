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


% Change
clc;
dirname = 'd:\PhD\Publications\2009-11_CVPR2010\Videos\Ducati\';
dirname2 ='d:\PhD\Publications\2009-11_CVPR2010\Videos\Ducati\';% mkdir(dirname2);
ext1 = 'png';
ext2 = 'png';

files = dir([dirname '*.' ext1]);
for i = 1:length(files)
    clc; disp([i length(files)]);
    img = imread([dirname files(i).name]);

    if size(img,1)~=420
    
        %img = imresize(img,0.25);
        %img = img(1:420,1:640,:);
        img = img(1:240,1:320,:);
        [pathname name ext] = fileparts(files(i).name);
        imwrite(img,[dirname2 name '.' ext2]);
    end
   
end
