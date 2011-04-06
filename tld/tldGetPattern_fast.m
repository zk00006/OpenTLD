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

function patt = tldGetPattern_faster(img, bbIn, patchsize)

num_bb = size(bbIn,2);
patt   = nan(prod(patchsize),num_bb);

for i = 1:num_bb
    bb = bbIn(:,i);
    cp = 0.5 * [bb(1)+bb(3); bb(2)+bb(4)]-1;
    
    sW = patchsize(1) / (bb(3)-bb(1)+1);
    sH = patchsize(2) / (bb(4)-bb(2)+1);
    
    S = [sW 0 0; 0 sH 0; 0 0 1];
    Sh = [1 0 -cp(1); 0 1 -cp(2); 0 0 1];
   
    H = S*Sh;
    
    ps2 = (patchsize(1)-1)/2;
    box = [-ps2 ps2 -ps2 ps2];
    
    patch = warp(img.blur,inv(H),box);
    
    patt(:,i) = patch(:) - mean(patch(:));
    
end