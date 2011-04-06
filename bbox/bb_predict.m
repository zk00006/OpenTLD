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


function [BB1 shift] = bb_predict(BB0,pt0,pt1)

of  = pt1 - pt0;
dx  = median(of(1,:));
dy  = median(of(2,:));

d1  = pdist(pt0','euclidean');
d2  = pdist(pt1','euclidean');
s   = median(d2./d1);

s1  = 0.5*(s-1)*bb_width(BB0);
s2  = 0.5*(s-1)*bb_height(BB0);

BB1  = [BB0(1)-s1; BB0(2)-s2; BB0(3)+s1; BB0(4)+s2] + [dx; dy; dx; dy];
shift = [s1; s2];
