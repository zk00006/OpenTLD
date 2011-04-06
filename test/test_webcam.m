obj    = videoinput('winvideo', 1, 'RGB24_320x240'); 

vidRes = get(obj, 'VideoResolution'); 
nBands = get(obj, 'NumberOfBands'); 
hImage = image( zeros(vidRes(2), vidRes(1), nBands) ); 
preview(obj, hImage); 