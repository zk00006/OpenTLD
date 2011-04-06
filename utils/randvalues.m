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

function out = randvalues(in,k)
% Randomly selects 'k' values from vector 'in'.

out = [];

N = size(in,2);

if k == 0
  return;
end

if k > N
  k = N;
end

if k/N < 0.0001
 i1 = unique(ceil(N*rand(1,k)));
 out = in(:,i1);
 
else
 i2 = randperm(N);
 out = in(:,sort(i2(1:k)));
end