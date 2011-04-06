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


function BB = bb_rescale_relative(BB,s)
% Change
BB = BB(1:4);
if length(s) == 1
    s = s*[1 1];
end
if isempty(BB), BB = []; return; end

s1 = 0.5*(s(1)-1)*bb_width(BB);
s2 = 0.5*(s(2)-1)*bb_height(BB);
BB = BB + [-s1; -s2; s1; s2];



