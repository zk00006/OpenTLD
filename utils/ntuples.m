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

function ntuple = ntuples(varargin)
% Computes all possible ntupples.

x = varargin;

ntuple = x{1};
for i = 2:length(x)
    num_col  = size(ntuple,2);
    num_item = length(x{i});
    ntuple   = repcel(ntuple, 1, num_item);
    newline  = repmat(x{i},1,num_col);
    ntuple   = [ntuple;newline];
end
