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


function bb_draw(bb1,varargin)
% Draw

if isempty(bb1), return; end;
idx = find(bb_isdef(bb1));
if isempty(varargin)
    varargin = {'edgecolor','y'};
end
for i = 1:length(idx)
    bb = bb1(:,idx(i));
    if bb(3)-bb(1) > 0 && bb(4)-bb(2)>0
        rectangle('Position',[bb(1) bb(2) bb(3)-bb(1) bb(4)-bb(2)],varargin{:})
        
        if size(bb,1) == 5
            cp = bb_center(bb);
            text(cp(1),cp(2),num2str(bb(5),3),'color','w');
        end
        
    end
end