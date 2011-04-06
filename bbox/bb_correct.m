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


function bb = bb_correct(img,bb)
% Change

K  = 2;
K2 = 1;
while 1

    clf; imshow(img); bb_rectangle(bb,'EdgeColor','y');
    %maximize();
    [x,y,ch] = ginput(1);
    if isempty(ch), break; end
    switch ch
        case 29 
            bb = [bb(1)+K; bb(2); bb(3)+K; bb(4)];
        case 28
            bb = [bb(1)-K; bb(2); bb(3)-K; bb(4)];
        case 30
            bb = [bb(1); bb(2)-K; bb(3); bb(4)-K];
        case 31
            bb = [bb(1); bb(2)+K; bb(3); bb(4)+K];
        case 46 % bigger
            bb = [bb(1)-K2; bb(2)-K2; bb(3)+K2; bb(4)+K2];
        case 44 % smaller
            bb = [bb(1)+K2; bb(2)+K2; bb(3)-K2; bb(4)-K2];
            
    end
    
end