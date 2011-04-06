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


clc; clf; clear all; warning off all;

path   = 'x:\PhD\Data\Papers\ECCV2008\seq2\';
% path   = 'X:\PhD\Data\Papers\Olcv2009\Motocross\';

files  = img_dir(path);
N      = length(files);
gtfile = [path 'gt.txt']; 
i      = 1;
boost  = 0;
play   = 0;

if ~exist(gtfile,'file')
    gt  = nan(4,N);
    img = imread([path files(1).name]);
    title('Initialize object: (1) left = top left, (2) right = bottom right');
    Gt = [149 110 29 33];
    %BB_first = [Gt(1) Gt(2) Gt(1)+Gt(3) Gt(2)+Gt(4)]';
     BB_first = bb_click(img);
    gt(:,1) = BB_first;
else
    gt = dlmread(gtfile)';
end

while 1
    maximize();

    img1 = imread([path files(i).name]);
    BB1  = gt(:,i);

    subplot(4,1,1:3); cla; imshow(img1); bb_rectangle(BB1,'EdgeColor','y','linewidth',3);
    title(['frame: ' num2str(i) '/' num2str(N) ', visible: ' num2str(sum(bb_isdef(gt))) ', occluded: ' num2str(sum(~bb_isdef(gt)))]);
    subplot(4,1,4); cla; hold on; plot(isfinite(gt(1,:)),'LineWidth',3);  plot(i,0.5,'o');
    title('left: next, right: back, up: correct, down: lost');
    drawnow;

    if boost == 0 && play == 0
        [x,y,ch] = ginput(1);
    elseif boost > 0
        boost = boost - 1; ch = 47;
    elseif play > 0
        play = play - 1; ch = 29;
    end

    % Interact
    switch ch
        case 47 % / : propagate
            if i < N
                gt(:,i+1) = track_lk(img1,imread([path files(i+1).name]),BB1,0);
                i = i + 1;
            end
        case 29 %right : next
            if i < N, i = i + 1; end
        case 28 % left : back
            if i > 1, i = i - 1; end
        case 30 % up : new
            subplot(4,1,4); title('Right click on object center (scale is fixef from first frame)');
            subplot(4,1,1:3);
            gt(:,i) = bb_click_move(img1,gt(:,1));
        case 31 % down : remove
            gt(:,i) = NaN;
        case 99 % C : correct
            subplot(4,1,1:3);
            gt(:,i) = bb_correct(img1,BB1);
        case 113 % Q : quit
            break;
        case 98 % B : boost
            boost = 20;
        case 101 % E : end
            i = N;
        case 102 % F : first
            i = 1; % input('Frame number: ');
        case 112 % P : play to the end
            play = 10;
    end
    if mod(i,10)==0, dlmwrite(gtfile,gt'); end
end

dlmwrite(gtfile,gt');