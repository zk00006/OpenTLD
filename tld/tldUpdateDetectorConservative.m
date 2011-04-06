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

function tld = tldUpdateDetectorConservative(tld,I)

bb    = tld.bb(:,I);
img   = tld.img{I};

% Consistency =============================================================

pPatt  = tldGetPattern(img,bb,tld.model.patchsize);
[pConf1,~,pIsin] = tldNN(pPatt,tld);

% Inconsistency: colildes with negative
if pConf1 < 0.5
%     disp('Fast change.');
    tld.valid(I) = 0;
    return;
end
if pIsin(3) == 1 || var(pPatt) < tld.var    
%     disp('In negative data.');
    return; 
end 

% If strong position, identify indexes that will be removed and updated
pRank  = pIsin(2);
nRank  = []; % indexes to patches that will be removed from PEX
nEx    = []; % patches that will be added to NEX
if ~isnan(pRank)
    
    id_far = bb_overlap(bb,tld.dt{I}.bb) < 0.2;
    id_inP = tld.dt{I}.isin(1,:) == 1;
    nRank  = tld.dt{I}.isin(2,id_far & id_inP);
    
    % Inconsistency: negative patterns is in PEX with lower rank
    if any(nRank<=pRank), 
        disp('inconsistent 2');
        return; 
    end 

    nEx = tld.dt{I}.patch(:,id_far);
end
tld.nEx{I} = nEx;
% Fern ====================================================================

% Generate positive patterns from current frame
overlap  = bb_overlap(bb,tld.grid);
[pX,pEx] = tldGeneratePositiveData(tld,overlap,img,tld.p_par_update);
pY       = ones(1,size(pX,2));

% Always use negative patterns from the first frame
% nX       = tld.X{1}(:,tld.Y{1}==0);
% idx      = pseudorandom_indexes(size(nX,2),10);
% nX       = nX(:,idx);

% If the current frame is valid, use negativ data from surrounding
% if pConf2 > tld.model.thr_nn_valid
idx = overlap < tld.n_par.overlap & tld.tmp.conf >= 0;

    %idx = pseudorandom_indexes(size(nX,2),10);
    %nX = nX(:,idx);

% Update fern
fern(2,[pX tld.tmp.patt(:,idx)],[pY zeros(1,sum(idx))],tld.model.thr_fern,2);

% Debug
if 0
    figure(10); clf;
    subplot(121);
    imshow(img.input);bb_draw(tld.dt{I}.bb); title('before update');
    subplot(122);
    fern(4,img,tld.control.maxbbox,tld.var,tld.tmp.conf,tld.tmp.patt);
    id_dt  = tld.tmp.conf > tld.model.thr_fern*tld.model.num_trees;
   
    imshow(img.input); bb_draw(tld.grid(:,id_dt));
end

% Nearest neighbour =======================================================

% Remove patterns from PEX
tld.pex(:,nRank) = [];

% Always use negative patches from the first frame
idx = pseudorandom_indexes(size(tld.nEx{1},2),10);
nEx = [nEx tld.nEx{1}(:,idx) tld.nEx{I-1}];


% Update NN
% TODO sort
tld = tldTrainNN(pEx,nEx,tld);

% sound(tld.gong(1:200));
