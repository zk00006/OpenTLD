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

function source = tldInitFirstFrame(tld,source,min_win)

% load the first frame into memory
source.im0  = img_get(source,source.idx(1));

% set the initial bounding box: 
% - from file
if source.camera == 0 && exist([source.input '/init.txt'],'file')
    bb = dlmread([source.input '/init.txt']);
    source.bb = bb(:);
    
    % check
    if isempty(source.bb) || min(bb_size(source.bb)) < min_win
        exit('Error: bounding box is incorrectly defined or too small');
    end
% - by mouse    
else
    source.bb = bb_click(tld,source.im0.input);
    
    % check
    if isempty(source.bb) || min(bb_size(source.bb)) < min_win
       source = [];
    end
end


