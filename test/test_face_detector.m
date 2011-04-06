% Face tracker
init_workspace;

o.name          = 'face_tracker';
o.overlap       = [0.4 0.7];
o.num_warps     = 100;
o.mirror        = 0;
o.angle         = 60;
o.f1            = feature_create('forest');
o.scanning      = struct('win_height', 1*24, 'win_width', 1*24, 'row_shift', 2, 'col_shift', 2, 'scale', 1.2, 'threshold', 2, 'min_std', 0, 'nms', 0, 'overlap',.4);
load('frontal.mat');


if ~isempty(vid)
    o.path  = '';
    o.idx   = 1:1000;
    i0      = img_alloc(getsnapshot(vid));
else
    o.path  = 'd:\PhD\Data\People\MultiFace\';
    o.files = img_dir(o.path);
    o.num   = length(o.files);
    o.idx   = repmat(1:o.num,1,1);
    i0      = img_alloc(o.files(o.idx(1)).name);
end

init_display;

profile on;
for i = 1:length(o.idx)
    i1 = img_get(o,i);
    
    bb = waldboost(i1.gray,model,o.scanning);
    [cBB,cSz] = bb_cluster(bb,0.5,3);
    
    set(iid,'cdata',i1.gray);
    h_rec = get(gca,'Children'); delete(h_rec(1:end-1));
    bb_draw([cBB;cSz],'linewidth',5,'edgecolor','y','curvature',[1 1]);
    drawnow;
    
    i0 = i1;
end
profile off; 
profile viewer;
