#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
#include <TLD.h>
using namespace cv;

Rect box;
bool drawing_box = false;
bool gotBB = false;	
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
    box = Rect( x, y, 0, 0 );
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
  if(argc<2){
    printf("use:\n     %s /path/parameters.yml\n",argv[0]);
    return 0;
  }
  //Init camera
  VideoCapture capture;
  capture.open(0);
  if (!capture.isOpened())
  {
	cout << "capture device failed to open!" << endl;
    return 1;
  }
  Mat frame;
  Mat last_gray;
  //Register mouse callback to draw the bounding box
  cvNamedWindow("TLD",CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback( "TLD", mouseHandler, NULL );
  //TLD framework
  TLD tld;
  //Read parameters file
  FileStorage fs(argv[1], FileStorage::READ);
  tld.read(fs.getFirstTopLevelNode());

  ///Initialization
GETBOUNDINGBOX:
gotBB=false;
  while(!gotBB)
  {
    capture >> frame;
    cvtColor(frame, last_gray, CV_RGB2GRAY);
    drawBox(frame,box);
    imshow("TLD", frame);
    if (cvWaitKey(33) == 'q')
	    return 0;
  }
  if (min(box.width,box.height)<24){
      cout << "Bounding box too small, try again." << endl;
      goto GETBOUNDINGBOX;
  }
  //Remove callback
  cvSetMouseCallback( "TLD", NULL, NULL );
  tld.init(last_gray,box);

  ///Run-time
  Mat current_gray;
  while(true){
    //get frame
    capture >> frame;
    cvtColor(frame, current_gray, CV_RGB2GRAY);
    //Track
    //tld.track(last_gray,current_gray,pts[0],pts[1]);
    //Detect using classifier
    //tld.detect()
    //Estimate errors
    //tld.evaluate();
    //Update classifier
    //tld.learn();
    //Draw Points
    //drawPoints(frame,pts[1]);
    //cout << "Tracked points: " << pts[1].size() << endl;
    //Display
    imshow("TLD", frame);
    //swap points and images
    swap(last_gray,current_gray);
    //std::swap(pts[0],pts[1]);
    if (cvWaitKey(33) == 'q')
      break;
  }
  return 0;
}
