import csv
import sys
import cv2

def box_size(b):
	return min(b[2]-b[0],b[3]-b[1])
	
boxes = csv.reader(open(sys.argv[1],'rb'),delimiter=',')
min_box=1000000
for box in boxes:
	if (box[0] != 'NaN'):
		box = map(float,box)
		if (box_size(box) < min_box):
			min_box = box_size(box)
	
print min_box
