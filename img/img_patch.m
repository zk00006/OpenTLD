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


function patch = img_patch(img, bb, randomize,p_par)

if nargin == 4 && randomize > 0
    
    rand('state',randomize);
    randn('state',randomize);
    
    NOISE = p_par.noise;
    ANGLE = p_par.angle;
    SCALE = p_par.scale;
    SHIFT = p_par.shift;
    
    cp  = bb_center(bb)-1;
    Sh1 = [1 0 -cp(1); 0 1 -cp(2); 0 0 1];
    
    sca = 1-SCALE*(rand-0.5);
    Sca = diag([sca sca 1]);
    
    ang = 2*pi/360*ANGLE*(rand-0.5);
    ca = cos(ang);
    sa = sin(ang);
    Ang = [ca, -sa; sa, ca];
    Ang(end+1,end+1) = 1;
    
    shR  = SHIFT*bb_height(bb)*(rand-0.5);
    shC  = SHIFT*bb_width(bb)*(rand-0.5);
    Sh2 = [1 0 shC; 0 1 shR; 0 0 1];
    
    bbW = bb_width(bb)-1;
    bbH = bb_height(bb)-1;
    box = [-bbW/2 bbW/2 -bbH/2 bbH/2];
    
    H     = Sh2*Ang*Sca*Sh1;
    bbsize = bb_size(bb);
    patch = uint8(warp(img,inv(H),box) + NOISE*randn(bbsize(1),bbsize(2)));
    
    
else
    
    % All coordinates are integers
    if sum(round(bb)-bb)==0
        L = max([1 bb(1)]);
        T = max([1 bb(2)]);
        R = min([size(img,2) bb(3)]);
        B = min([size(img,1) bb(4)]);
        patch = img(T:B,L:R);
        
        % Sub-pixel accuracy
    else
        
        cp = 0.5 * [bb(1)+bb(3); bb(2)+bb(4)]-1;
        H = [1 0 -cp(1); 0 1 -cp(2); 0 0 1];
        
        bbW = bb(3,:)-bb(1,:);
        bbH = bb(4,:)-bb(2,:);
        if bbW <= 0 || bbH <= 0
            patch = [];
            return;
        end
        box = [-bbW/2 bbW/2 -bbH/2 bbH/2];
        
        if size(img,3) == 3
            for i = 1:3
                P = warp(img(:,:,i),inv(H),box);
                patch(:,:,i) = uint8(P);
            end
        else
            patch = warp(img,inv(H),box);
            patch = uint8(patch);
        end

    end
end


