clf; clc; clear all; warning off;

i0 = imread('tire.tif');
bb =  [81    80   200   200]';
patchsize = [10 10];


i2 = img_blur(i0,10);
tic
p1 = tldPatch2Pattern_faster(i2,bb,patchsize);
toc

tic
p2 = img_patch(i0,bb);
p2 = imresize(p2,patchsize);
toc

subplot(121); imshow(p1,[]);
subplot(122); imshow(p2,[]);



