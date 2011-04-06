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

function tld = tldUpdateDetector(tld,I)

bb    = tld.bb(:,I);
conf  = tld.conf(:,I);
img   = tld.img{I};

% Fern ====================================================================

% Generate positive patterns from current frame
overlap = bb_overlap(bb,tld.grid);
[pX,pEx] = tldGeneratePositiveData(tld,overlap,img,tld.p_par_update);
pY = ones(1,size(pX,2));
if isempty(pX), return, end

% Forbidden list
%idx_KO = tld.dt{I}.idx(tld.dt{I}.conf > conf);
%idx(idx_KO) = 0;

% Always use negative patterns from the first frame
nX = tld.X{1}(:,tld.Y{1}==0);
num_nX = size(nX,2);
% idx = randvalues(1:num_nX,num_nX/10);
idx = pseudorandom_indexes(num_nX,10);
nX = nX(:,idx);

% If the current frame is valid, use negativ data from surrounding
if conf > tld.model.thr_nn_valid
    idx = overlap < tld.n_par.overlap & tld.tmp.conf > 0;
    nX = [nX tld.tmp.patt(:,idx)];
end
nY = zeros(1,size(nX,2));

% Update fern
fern(2,[pX nX],[pY nY],tld.model.thr_fern,2);

% Debug
if tld.debug
    disp(['Fern update, frame: ' num2str(I)]);
    disp(['# P pattern: ' num2str(size(pX,2))]);
    disp(['# N pattern: ' num2str(size(nX,2))]);
    figure(10);
    subplot(121);
    imshow(img.input);bb_draw(tld.dt{I}.bb); title('before update');
    subplot(122);
    fern(4,img,tld.control.maxbbox,tld.var,tld.tmp.conf,tld.tmp.patt);
    id_dt  = tld.tmp.conf > tld.thr_fern*tld.num_trees;
    imshow(img.input); bb_draw(tld.grid(:,id_dt));
end

% Nearest neighbour =======================================================

% Always use negative patches from the first frame
num_nEx = size(tld.nEx{1},2);
%idx = randvalues(1:num_nEx,round(num_nEx/100));
idx = pseudorandom_indexes(num_nEx,100);
nEx = tld.nEx{1}(:,idx);

% If current frame is valid, use negative patterns from surrounding
if conf > tld.model.thr_nn_valid
    overlap = bb_overlap(bb,tld.dt{I}.bb);
    idx     = overlap < tld.n_par.overlap;
    nEx     = [nEx tld.dt{I}.patch(:,idx)];
end

% Update NN
tld = tldTrainNN(pEx,nEx,tld);


% Bootstrap
% idx = randi(size(tld.X{1},2),1000,1);
% fern(2,tld.X{1},tld.Y{1},1,1,idx);


