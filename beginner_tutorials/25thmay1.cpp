#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
 
using namespace std;
using namespace cv;

Mat denoising(Mat thresh) ;
int centroid(Mat thresholded,Mat src,int A);
Mat contour_detection(Mat threshold ,Mat src);
 
int main(int argc, char* argv[])
{
        VideoCapture cap(1);
   if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return -1;
    }
    
namedWindow("src",1);

 while(1)
    {
     
    Mat src;
    bool bSuccess = cap.read(src); // read a new frame from video
    if (!bSuccess) //if not success, break loop
    break;
       
    //converting rgb to differtinal rgb
    Mat rgbChannels[3];
    split(src, rgbChannels);       // splitting image
    Mat g,new_rgb,diff_red,diff_blue,diff_green;
    g = Mat::ones(Size(src.cols, src.rows), CV_8UC1);
    diff_red=(rgbChannels[2]-rgbChannels[1])+g;
    diff_blue=(rgbChannels[0]-rgbChannels[2])+g;     //algorithm implementing for differential rgb.
    diff_green=(rgbChannels[1]-rgbChannels[0])+g;  
    vector<Mat> channels;
    channels.push_back(diff_blue);
    channels.push_back(diff_green);
    channels.push_back(diff_red);
    merge(channels, new_rgb);     // merging image
    
    // thresholding by layer
    // blue colour threshold.

     Mat threshold_blue;
     threshold_blue=diff_blue > 83 & diff_blue < 210 & diff_green > 0 & diff_green < 45 & diff_red> 0 & diff_red< 45;
     threshold_blue = denoising(threshold_blue); //denoising
     int blue=1;
     centroid(threshold_blue,src,blue); //centroid calculation
 
    // yellow colour threshold.
     Mat threshold_yellow;
     threshold_yellow=diff_blue  > 0 & diff_blue  < 20 & diff_green > 54 & diff_green < 144 & diff_red> 7 & diff_red < 52;
     threshold_yellow = denoising(threshold_yellow); //denoising
     int yellow=2;
     centroid(threshold_yellow,src,yellow); // centroid calculation

    // black colour threshold.
      Mat threshold_black,hsv;
      cvtColor(src, hsv, COLOR_BGR2HSV);
      inRange(hsv, Scalar(0,0,58), Scalar(179,66,110),threshold_black); 
      threshold_black = denoising(threshold_black); //denoising
      threshold_black=contour_detection(threshold_black,src);
      int black=3;
      centroid(threshold_black,src,black); //centroid calculation

  
       
     //display images.
    
     imshow("src",src);
     imshow("new_rgb",new_rgb);


     if(waitKey(3) == 27) 
        break; 
      
    }
return 0;
}
     Mat denoising(Mat thresh) 
{
     erode(thresh,thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
     dilate( thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
     dilate( thresh,thresh, getStructuringElement(MORPH_ELLIPSE,Size(5, 5)) ); 
     erode(thresh, thresh, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
     return thresh;
}
     int centroid(Mat thresholded,Mat src,int A)
{

  Moments oMoments = moments(thresholded);
  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;
  // if the area <= 1000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  if (dArea > 1000)
  {
   //calculate the position 
   
   int posX= dM10 / dArea;
    int posY= dM01 / dArea;        
        
      if (posX >= 0 && posY >= 0)
      {
    //Draw a red line from the previous point to the current point
          circle(thresholded, Point(posX,posY),10,Scalar(0,255,0),2);
          if (A==1) 
             { 
             cout << "blue - " << "x = "<< posX << " y = " << posY << endl ;
             imshow(" thresholded ",thresholded);
             }
          else if (A==2)
             { 
             cout << "yellow - " << "x = "<< posX << " y = " << posY << endl ;
             imshow(" thresholded ",thresholded);
             }
          else if(A==3)
             {
             cout << "black - " << "x = "<< posX << " y = " << posY << endl ;
             imshow(" thresholded ",thresholded);
             } 
       }
    
   }

 return 0;
}
Mat contour_detection(Mat threshold,Mat src)
{

 int largest_area=0;
 int largest_contour_index=0;
 Rect bounding_rect; 
 Mat dst(src.rows,src.cols,CV_8UC1,Scalar::all(0));
  
    vector< vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
 
    findContours( threshold, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
   Scalar color( 255,255,255);
     for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
      {
       drawContours( dst, contours,i, color, CV_FILLED, 8, hierarchy ); 
       
       }
   
    
 
 
 imshow( "contour", dst );
 return dst;

 }





