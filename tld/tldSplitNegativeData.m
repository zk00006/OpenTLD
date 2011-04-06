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

function [nX1,nX2,nEx1,nEx2] = tldSplitNegativeData(nX,nEx)
% Splits negative data to training and validation set

N    = size(nX,2);
idx  = randperm(N);
nX   = nX(:,idx);
nX1  = nX(:,1:N/2); 
nX2  = nX(:,N/2+1:end); 

N    = size(nEx,2);
idx  = randperm(N);
nEx  = nEx(:,idx);
nEx1 = nEx(:,1:N/2); 
nEx2 = nEx(:,N/2+1:end);