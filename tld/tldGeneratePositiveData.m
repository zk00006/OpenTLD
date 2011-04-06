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

function [pX,pEx,bbP0] = tldGeneratePositiveData(tld,overlap,im0,p_par)

pX   = [];
pEx  = [];%zeros(prod(tld.patchsize),numWarps);

% Get closest bbox
[~,idxP] = max(overlap);
bbP0 =  tld.grid(1:4,idxP);

% Get overlapping bboxes
idxP = find(overlap > 0.6);
if length(idxP) > p_par.num_closest
    [~,sIdx] = sort(overlap(idxP),'descend');    
    idxP = idxP(sIdx(1:p_par.num_closest));
end
bbP  = tld.grid(:,idxP);
if isempty(bbP), return; end

% Get hull
bbH  = bb_hull(bbP);
cols = bbH(1):bbH(3);
rows = bbH(2):bbH(4);

im1 = im0;
pEx = tldGetPattern(im1,bbP0,tld.model.patchsize);
if tld.model.fliplr
pEx = [pEx tldGetPattern(im1,bbP0,tld.model.patchsize,1)];
end
for i = 1:p_par.num_warps
    if i > 1
        randomize = rand; % Sets the internal randomizer to the same state
        %patch_input = img_patch(im0.input,bbH,randomize,p_par);
        patch_blur = img_patch(im0.blur,bbH,randomize,p_par);
        im1.blur(rows,cols) = patch_blur;
        %im1.input(rows,cols) = patch_input;
    end
    
    % Measures on blured image
    pX  = [pX fern(5,im1,idxP,0)];
    
    % Measures on input image
    %pEx(:,i) = tldGetPattern(im1,bbP0,tld.model.patchsize);
    %pEx = [pEx tldGetPattern(im1,tld.grid(1:4,idxP),tld.model.patchsize)];
    
end