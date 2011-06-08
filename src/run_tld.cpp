#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
#include <LKTracker.h>
using namespace cv;

CvRect box;
bool drawing_box = false;
bool gotBB = false;	
Mat last_gray;
//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
  switch( event ){
    case CV_EVENT_MOUSEMOVE: 
      if( drawing_box ){
        box.width = x-box.x;
        box.height = y-box.y;
      }
      break;
	case CV_EVENT_LBUTTONDOWN:
      drawing_box = true;
      box = cvRect( x, y, 0, 0 );
      break;
    case CV_EVENT_LBUTTONUP:
      drawing_box = false;
      if( box.width < 0 ){
        box.x += box.width;
        box.width *= -1;
      }
      if( box.height < 0 ){
        box.y += box.height;
        box.height *= -1;
      }
      gotBB = true;
      break;
	}
}

int main(int argc, char * argv[]){
  //Init camera
  VideoCapture capture;
  capture.open(0);
  if (!capture.isOpened())
  {
	cout << "capture device failed to open!" << endl;
    return 1;
  }
  Mat frame;
  //Register mouse callback to draw the bounding box
  cvNamedWindow("TLD",CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback( "TLD", mouseHandler, NULL );
  //Feature Detector: FAST corner detector
  DynamicAdaptedFeatureDetector detector(new FastAdjuster(20,true),80,120,10);
  //Tracker
  LKTracker tracker;
  //Classifier
  //FernClassifier classifier;
  ///Initialization
  //Get the Bounding Box
  while(!gotBB)
  {
    capture >> frame;
    cvtColor(frame, last_gray, CV_RGB2GRAY);
    drawBox(frame,box);
    imshow("TLD", frame);
    if (cvWaitKey(33) == 'q')
	    break;
  }
  //remove callback
  cvSetMouseCallback( "TLD", NULL, NULL );
  //Extract Features within the Bounding Box
  vector<KeyPoint> kpts;
  Mat mask = createMask(last_gray,box);
  detector.detect(last_gray,kpts,mask);
  vector<Point2f> pts[2];
  KeyPoint::convert(kpts,pts[0]);
  cout << "Puntos iniciales detectados:" << pts[0].size() << endl;
  //save init frame with detected points
  drawPoints(frame,pts[0]);
  imwrite("init.jpg",last_gray);
  //Train classifier

  //fernClassifier.train(gray,box,params);
  ///Run-time
  Mat current_gray;
  while(true){
    //get frame
    capture >> frame;
    cvtColor(frame, current_gray, CV_RGB2GRAY);
    //Forward-Backward frame-to-frame tracking
    tracker.trackf2f(last_gray,current_gray,pts[0],pts[1]);
    //Draw Points
    drawPoints(frame,pts[1]);
    cout << "Tracked points: " << pts[1].size() << endl;
    //evaluate classifier
    //estimate errors
    //update classifier
    //display
    imshow("TLD", frame);
    //swap points and images
    swap(last_gray,current_gray);
    std::swap(pts[0],pts[1]);
    if (cvWaitKey(33) == 'q')
      break;
  }
  return 0;
}
