clf; clc; clear all; warning off;

i0 = imread('tire.tif');
bb =  [80.1    80   200   200]';

tic

 profile on
for i = 1:1000
p0 = i0(bb(2):bb(4),bb(1):bb(3));
end
toc
tic
for i = 1:1000
p1 = img_patch(i0,bb);
end
toc
 profile off
profile viewer;

subplot(131); imshow(p0-p1,[]);
subplot(132); imshow(p0,[]);
subplot(133); imshow(p1,[]);


