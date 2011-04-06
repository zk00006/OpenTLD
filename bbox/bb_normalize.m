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


function bb2 = bb_normalize(bb1,bb0)
% Change
% Shifts and rescales bboxes in BB1 according to the first bbox in BB0.

W = bb_width( bb0(:,1)) / bb_width(bb1(:,1));
H = bb_height( bb0(:,1)) / bb_height(bb1(:,1));
SH = bb_center( bb0(:,1)) - bb_center(bb1(:,1));

bb2 = bb_rescale(bb_shift(bb1,SH),[W,H]);

