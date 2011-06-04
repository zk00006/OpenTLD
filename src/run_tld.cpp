#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
using namespace cv;

CvRect box;
bool drawing_box = false;
bool gotBB = false;	
Mat gray;
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
  //Initialization
  while(!gotBB)
  {
    //get frame
    capture >> frame;
    cvtColor(frame, gray, CV_RGB2GRAY);
    drawBox(gray,box);
    imshow("TLD", gray);
    if (cvWaitKey(33) == 'q')
	    break;
  }
  //remove callback
  cvSetMouseCallback( "TLD", NULL, NULL );
  //save init frame
  imwrite("init.jpg",gray);    
  //train classifier
  //train(gray,box,params);
  //Run-time
  while(true)
  {
	  //get frame
      capture >> frame;
      cvtColor(frame, gray, CV_RGB2GRAY);
      //evaluate classifier
      //estimate errors
	  //update classifier
      //display
      imshow("TLD", gray);
      if (cvWaitKey(33) == 'q')
	    break;
  }
  return 0;
}
