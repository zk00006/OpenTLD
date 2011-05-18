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


function images = img_dir(path)
% Looks into 'path' directory and returns a list of images in it

% considered extensions
ext = {'*.jpeg','*.jpg','*.png','*.pgm'};

images = [];
for i = 1:length(ext)
    if exist('OCTAVE_VERSION','builtin')
        dir_list = dir([path ext{i}]);
        if length(dir_list) > 0
            images = [images dir_list];
        end
    else
        images = [images dir([path ext{i}])];
    end
end

% images are returned with absolute path
for i = 1:length(images)
    images(i).name = [path images(i).name];
end
