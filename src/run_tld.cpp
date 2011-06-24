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

  VideoCapture capture;
  string video;
  switch (argc){
  case 4:
    if (strcmp(argv[2],"-s")==0){
        video = string(argv[3]);
        capture.open(video);
    }
    break;
  case 2:
    capture.open(0);
    break;
  default:
    printf("use:\n     %s /path/parameters.yml [-s source]\n",argv[0]);
    return 0;
    break;
  }

  //Init camera

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
  BoundingBox pbox;
  vector<Point2f> pts1;
  vector<Point2f> pts2;
  bool status=true;
  while(true){
    //get frame
    capture >> frame;
    cvtColor(frame, current_gray, CV_RGB2GRAY);
    //Process Frame
    tld.processFrame(last_gray,current_gray,pts1,pts2,pbox,status);
    //Draw Points
    if (status){
      drawPoints(frame,pts1);
      drawPoints(frame,pts2,Scalar(0,255,0));
      drawBox(frame,pbox);
    }
    //Display
    imshow("TLD", frame);
    //swap points and images
    swap(last_gray,current_gray);
    pts1.clear();
    pts2.clear();
    if (cvWaitKey(33) == 'q')
      break;
  }
  return 0;
}
