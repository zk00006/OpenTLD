clc; 

S = [22  30];
bb = [1 1 15 15]';

grid = bb_scan(bb,S)
%  grid = grid(:,1:1000);
%  o = bb_overlap(grid,grid,2);