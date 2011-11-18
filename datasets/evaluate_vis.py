import csv
import sys
import cv
import cv2
def bb_overlap(bb1,bb2,frame):
	if (bb1[0] == 'NaN' or bb2[0] == 'NaN'):
		return 0
	if (bb1[0]>bb2[2]):
		return 0
	if (bb1[1]>bb2[3]):
		return 0
	if (bb1[2]<bb2[0]):
		return 0
	if (bb1[3]<bb2[1]):
		return 0
	x1 = max(bb1[0],bb2[0])
	x2 = min(bb1[2],bb2[2])
	y1 = max(bb1[1],bb2[1])
	y2 = min(bb1[3],bb2[3])
	colInt = x2-x1
	rowInt = y2-y1
	frame[y1:y2,x1:x2]=(100,200,100)
	intersection = colInt*rowInt
	area1 = (bb1[2]-bb1[0])*(bb1[3]-bb1[1])
	area2 = (bb2[2]-bb2[0])*(bb2[3]-bb2[1])
	overlap = intersection/(area1 + area2 - intersection)
	#if (overlap<0.5):
	#	print "overlap %f" %overlap
	return overlap
def drawBox(frame,box,color):
	cv2.rectangle(frame,(int(box[0]),int(box[1])),(int(box[2]),int(box[3])),color,1)
	
cap = cv2.VideoCapture(sys.argv[1])
#size = (int(cap.get(cv.CV_CAP_PROP_FRAME_WIDTH)),int(cap.get(cv.CV_CAP_PROP_FRAME_HEIGHT)))
#rec = cv2.VideoWriter("evaluation.avi",cv.CV_FOURCC('P','I','M','1'),30,size,1)
boxes = csv.reader(open(sys.argv[2],'rb'),delimiter=',')
true_boxes = csv.reader(open(sys.argv[3],'rb'),delimiter=',')
correct_detections = 0.0
detections = 0.0
true_detections = 0.0

while (1):
	ret,frame= cap.read()
	if (ret!=1):
		break
	true_box = true_boxes.next()
	box = boxes.next()
	if (box[0] != 'NaN'):
		box = map(float,box)
		drawBox(frame,box,(0,0,255))
		detections=detections+1
	if (true_box[0] != 'NaN'):
		true_detections=true_detections +1
		true_box = map(float,true_box)
		drawBox(frame,true_box,(0,255,0))
	if (bb_overlap(box,true_box,frame)>0.25):
		correct_detections = correct_detections + 1
	cv2.imshow("Evaluation",frame)
	#rec.write(frame)
	cv2.waitKey(30)
print "detections = %f" % detections
print "true detections = %f" % true_detections
print "correct detections = %f" % correct_detections
precision = correct_detections/detections
recall = correct_detections/true_detections
f_measure =  2*precision*recall/(precision+recall) if (precision >0 or recall >0) else 0
print "precision=%f" %  precision
print "recall=%f" % recall
print "f-measure= %f" % f_measure
