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

function [bb,cSz] = bb_cluster(bb2, SPACE_THR, MIN_NUM_BB)
% Clusters bboxes.

if isempty(bb2)
    bb = [];
    cSz = [];
    return;
end

switch size(bb2,2)
    case 0, T = [];
    case 1, T = 1;
    case 2
        T = ones(2,1);
        if bb_distance(bb2) > SPACE_THR, T(2) = 2; end
    otherwise
        bbd = bb_distance(bb2);
        Z = linkagemex(bbd,'si');
        if exist('OCTAVE_VERSION','builtin')
            T = cluster_oct(Z,'cutoff', SPACE_THR,'criterion','distance');
        else
            T = cluster(Z,'cutoff', SPACE_THR,'criterion','distance');
        end
end
uT = unique(T);

% Merge clusters
bb = [];
cSz = [];
for i = 1:length(uT)
    num_bb = sum(T == uT(i));
    if num_bb >= MIN_NUM_BB
        bb = [bb mean(bb2(:,T == uT(i)),2)];
        cSz = [cSz num_bb];
    end
end
