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

% Source for TLD demo (CVPR'10)
% Usage: 
%  1. Define input
%     a) plug in your camera
%     b) fill '/_input' with image sequence, define 'init.txt' (initial bounding box position)
%  2. run demo.m
%  3. Output of the TLD is written to 'log.txt', format [frame id; left col; top row; right col; bottom row]
%   
% 
% Control keys:
%    n ... shows negative examples in online model (default on)
%    p ... shows positive examples in online model (default on)
%    i ... initialization of different target
%    c ... show confidence score (default on)
%    o ... show output as circle/dot/no output (default circle)
%    d ... show detections (default on)
%    t ... show target in top left corner (default off)
%    r ... replace target with first patch (default off)
%    # ... draw trajectory of target (default off)
%    <space> ... svae current image
%    1 ... mode without learning (fast, does not improve)
%    2 ... mode with learning (slower, but can learn new appearances and discriminate)
%    q ... quit application

addpath(genpath('.')); init_workspace; 

% Input
opt.source          = struct('camera',0,'input','_input/','bb0',[]);
opt.debug           = 0;
opt.name            = '_snapshots/'; mkdir(opt.name); % saves s snapshot every 100th frames

% Parameters
opt.model           = struct('ncc_thesame',0.95,'valid',0.5,'patchsize',[10 10],'min_win',20,'num_trees',10,'num_features',13,'thr_fern',0.5,'thr_nn',0.65,'thr_nn_valid',0.7,'fliplr',0);
opt.p_par_init      = struct('num_closest',10,'num_warps',20,'noise',5,'angle',20,'shift',0.02,'scale',0.02);
opt.p_par_update    = struct('num_closest',10,'num_warps',10,'noise',5,'angle',10,'shift',0.02,'scale',0.02);
opt.n_par           = struct('overlap',0.2,'num_patches',100,'num_synthetic',0,'noise',1);
opt.tracker         = struct('occlusion',10,'grid',10,'big_fb',10);
opt.control         = struct('maxbbox',.9,'update_detector',1,'drop_img',1,'repeat',1,'rescale',1);
opt.plot            = struct('pex',1,'nex',1,'save',0,'dt',0,'confidence',0,'target',0,'replace',0,'drawoutput',3,'draw',0,'pts',0,'help', 0,'patch_rescale',1);

%  profile on;
global tld;
tldDemo(opt);
%   profile off;
%   profile viewer;