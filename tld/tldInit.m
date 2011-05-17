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

function tld = tldInit(opt,tld)

lk(0);

if ~isempty(tld);
    handle = tld.handle;
    tld = opt;
    tld.handle = handle;
else
    tld = opt;
end


% INITIALIZE DETECTOR =====================================================

% Scanning grid
[tld.grid tld.scales] = bb_scan(tld.source.bb,size(tld.source.im0.input),tld.model.min_win);

% Features
tld.nGrid     = size(tld.grid,2);
tld.features  = tldGenerateFeatures(tld.model.num_trees,tld.model.num_features,0);

% Initialize Detector
fern(0); % cleanup
fern(1,tld.source.im0.input,tld.grid,tld.features,tld.scales); % allocate structures

% Temporal structures
tld.tmp.conf = zeros(1,tld.nGrid);
tld.tmp.patt = zeros(tld.model.num_trees,tld.nGrid);

% RESULTS =================================================================

% Initialize Trajectory
tld.img     = cell(1,length(tld.source.idx));
tld.snapshot= cell(1,length(tld.source.idx));
tld.dt      = cell(1,length(tld.source.idx));
tld.bb      = nan(4,length(tld.source.idx));
tld.conf    = nan(1,length(tld.source.idx));
tld.valid   = nan(1,length(tld.source.idx));
tld.size    = nan(1,length(tld.source.idx));
tld.trackerfailure = nan(1,length(tld.source.idx));
tld.draw    = zeros(2,0);
tld.pts     = zeros(2,0);
% Fill first fields
tld.img{1}  = tld.source.im0;
tld.bb(:,1) = tld.source.bb;
tld.conf(1) = 1;
tld.valid(1)= 1;
tld.size(1) = 1;

% TRAIN DETECTOR ==========================================================

% Initialize structures
tld.imgsize = size(tld.source.im0.input);
tld.X       = cell(1,length(tld.source.idx)); % training data for fern
tld.Y       = cell(1,length(tld.source.idx)); 
tld.pEx     = cell(1,length(tld.source.idx)); % training data for NN
tld.nEx     = cell(1,length(tld.source.idx));
overlap     = bb_overlap(tld.source.bb,tld.grid); % bottleneck

% Target (display only)
tld.target = img_patch(tld.img{1}.input,tld.bb(:,1));

% Generate Positive Examples
[pX,pEx,bbP] = tldGeneratePositiveData(tld,overlap,tld.img{1},tld.p_par_init);
pY = ones(1,size(pX,2));
% disp(['# P patterns: ' num2str(size(pX,2))]);
% disp(['# P patches : ' num2str(size(pEx,2))]);

if opt.PRINT_DEBUG==1
    fprintf('Original Bounding Box: (%f,%f) (%f,%f)\n',tld.source.bb(1),tld.source.bb(2),tld.source.bb(3),tld.source.bb(4));
end
% Correct initial bbox
tld.bb(:,1) = bbP(1:4,:);
if opt.PRINT_DEBUG==1
    fprintf('New Bounding Box: (%f,%f) (%f,%f)\n',bbP(1),bbP(2),bbP(3),bbP(4));
end

% Variance threshold
tld.var = var(pEx(:,1))/2;
if opt.PRINT_DEBUG==1
    fprintf('Variance : %s\n', num2str(tld.var));
end

% Generate Negative Examples
[nX,nEx] = tldGenerateNegativeData(tld,overlap,tld.img{1});
% disp(['# N patterns: ' num2str(size(nX,2))]);
% disp(['# N patches : ' num2str(size(nEx,2))]);

% Split Negative Data to Training set and Validation set
[nX1,nX2,nEx1,nEx2] = tldSplitNegativeData(nX,nEx);
nY1  = zeros(1,size(nX1,2));

% Generate Apriori Negative Examples
%[anX,anEx] = tldGenerateAprioriData(tld);
%anY = zeros(1,size(anX,2));
% disp(['# apriori N patterns: ' num2str(size(anX,2))]);
% disp(['# apriori N patches : ' num2str(size(anEx,2))]);

tld.pEx{1}  = pEx; % save positive patches for later
tld.nEx{1}  = nEx; % save negative patches for later
tld.X{1}    = [pX nX1];
tld.Y{1}    = [pY nY1];
idx         = randperm(size(tld.X{1},2));
tld.X{1}    = tld.X{1}(:,idx);
tld.Y{1}    = tld.Y{1}(:,idx);

% Train using training set ------------------------------------------------

% Fern
bootstrap = 2;
fern(2,tld.X{1},tld.Y{1},tld.model.thr_fern,bootstrap);

% Nearest Neightbour 

tld.pex = [];
tld.nex = [];

tld = tldTrainNN(pEx,nEx1,tld);
tld.model.num_init = size(tld.pex,2);

% Estimate thresholds on validation set  ----------------------------------

% Fern
conf_fern = fern(3,nX2);
tld.model.thr_fern = max(max(conf_fern)/tld.model.num_trees,tld.model.thr_fern);
if opt.PRINT_DEBUG==1
    fprintf('Fern Threshold: %f\n',tld.model.thr_fern);
end

% Nearest neighbor
conf_nn = tldNN(nEx2,tld);
tld.model.thr_nn = max(tld.model.thr_nn,max(conf_nn));
tld.model.thr_nn_valid = max(tld.model.thr_nn_valid,tld.model.thr_nn);

if opt.PRINT_DEBUG==1
    fprintf('Nearest Neighbor Threshold: %f\n',tld.model.thr_nn);
end

% Save ground truth data
if (opt.SAVEGROUNDTRUTH==1)
    grid = tld.grid; %#ok<NASGU>
    features = tld.features; %#ok<NASGU>
    pex = tld.pex; %#ok<NASGU>
    nex = tld.nex; %#ok<NASGU>

    save([opt.output 'grid.txt'],'grid','-ascii')
    save([opt.output 'feature.txt'],'features') %not possible to save as an ascii file

    save([opt.output 'pex.txt'],'pex','-ascii')
    save([opt.output 'nex.txt'],'nex','-ascii')

    save([opt.output 'pX.txt'],'pX','-ascii')
    save([opt.output 'nX1.txt'],'nX1','-ascii')
    save([opt.output 'nX2.txt'],'nX2','-ascii')

    save([opt.output 'pEx.txt'],'pEx','-ascii')
    save([opt.output 'nEx1.txt'],'nEx1','-ascii')
    save([opt.output 'nEx2.txt'],'nEx2','-ascii')

    save([opt.output 'conf_fern.txt'],'conf_fern','-ascii')
    save([opt.output 'conf_nn.txt'],'conf_nn','-ascii')
    
    fprintf('\nGround truth data saved!\n\n');
end
