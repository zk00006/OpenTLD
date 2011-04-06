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


function bb2 = bb_square(bb1)
% Makes all bboxes square with the same area.

if isempty(bb1);
    bb2 = [];
    return;
end

S = bb_scale(bb1);
C = bb_center(bb1);

bb2 = [C(1,:) - S/2; C(2,:) - S/2; C(1,:) + S/2; C(2,:) + S/2];
