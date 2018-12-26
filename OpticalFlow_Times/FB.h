/*This library was created by Nicolas Ortiz and the research group on Image Processing of the 
Universidad Piloto de Colombia.  
*/

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include <sstream>


using namespace cv;
using namespace std;

String convert(int i, String name){
	std::stringstream sstm;
	String result;
	sstm << i << name;
	result = sstm.str();
	return result;
}

/*Transform the image to the XYZ 
 */

Mat RGBtoXYZ(Mat image){

	double minR, maxR, minG, maxG, minB, maxB, intensityR, intensityG, intensityB, m, n;
	Mat imageR, imageG, imageB, imageXYZ;
	//vector<Mat> channels(3);
	Mat channels[3];
	split(image,channels);
	imageR=channels[0];
	imageG=channels[1];
	imageB=channels[2];
	for(n=1;n==image.cols;n++){
		for(m=1;m==image.rows;m++){

			intensityR=imageR.at<uchar>(n,m);
			intensityG=imageG.at<uchar>(n,m);
			intensityB=imageB.at<uchar>(n,m);
			imageR.at<uchar>(n,m)=intensityR/(intensityR+intensityG+intensityB);
			imageG.at<uchar>(n,m)=intensityG/(intensityR+intensityG+intensityB);
			imageB.at<uchar>(n,m)=intensityB/(intensityR+intensityG+intensityB);
			
		}
	}
	minMaxLoc(imageR,&minR,&maxR);
	minMaxLoc(imageG,&minG,&maxG);
	minMaxLoc(imageB,&minB,&maxB);
	imageR=(255/maxR)*imageR;
	imageG=(255/maxG)*imageG;
	imageB=(255/maxB)*imageB;
	channels[0]=imageR;
	channels[1]=imageG;
	channels[2]=imageB;			
	merge(channels,3,imageXYZ);
	return imageXYZ;
}

/* Drawing Method for the Farnerbäck Algorythm Mixed with the substraction Algorythm
*/

Mat drawOptFlowMap (const Mat& flow, Mat& cflowmap, int step, const Scalar& color) {

	double VM, Phi, X1, X0, Y1, Y0, X, Y, L, i;
	VM=0; Phi=0; X=0; Y=0; i=0;
	
	for(int y = 0; y < cflowmap.rows; y += step){
	
	        for(int x = 0; x < cflowmap.cols; x += step){
	
			const Point2f& fxy = flow.at< Point2f>(y, x);
            		line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),color);
            		circle(cflowmap, Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), 3, color, -1);
			X1=x+fxy.x; Y1=y+fxy.y;
			X0=x; Y0=y;

			if(X1!=X0 && Y1!=Y0){
				if(sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0))<25){
					VM=VM + sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
					Phi=Phi+atan((Y1-Y0)/(X1-X0));	
					X=X+X1; Y=Y+Y1;
					i=i+1;
				}
			}

        	}

	}

	VM=VM/i; Phi=Phi/i;
	X=X/i; Y=Y/i; 
	double Xnew=X+VM*cos(Phi); double Ynew=Y+VM*sin(Phi);	
	CvPoint p0 =cvPoint(cvRound(X),cvRound(Y));
	CvPoint p1 =cvPoint(cvRound(Xnew),cvRound(Ynew));
	circle( cflowmap, p0, 8, Scalar(255,0,0), -1, 8);
	circle( cflowmap, p1, 8, Scalar(0,0,255), -1, 8);
	std::ostringstream strs;
	strs << VM;
	std::string str = strs.str();
	std::ostringstream strs2;
	strs2 << Phi;
	std::string str2 = strs2.str();
	putText(cflowmap, "Magnitude: "+str, Point2f(X,Y),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);
	putText(cflowmap, "Angle: "+str2, Point2f(X,Y-30),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);
	line(cflowmap,p0,p1,CV_RGB(0,0,0),5,8);

	return cflowmap;

}

/* Farnerbäck Optical Flow mixed with the subtraction Algorythm
*/

Mat FB(Mat subs, Mat prevGray, Mat gray, Mat image){

	Mat kernel= Mat::ones(51, 51, CV_8UC1);
	TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,100,0.03);
	Size subPixWinSize(7,7), winSize(31,31);
	Point2f vel;	
	const int MAX_COUNT=1000;
	double min, max;
	Mat flow, subs1, subs2;
	subs=RGBtoXYZ(subs);
	cvtColor(subs, subs, COLOR_BGR2GRAY);
	prevGray=RGBtoXYZ(prevGray);
	cvtColor(prevGray, prevGray, COLOR_BGR2GRAY);
	gray=RGBtoXYZ(gray);
	cvtColor(gray, gray, COLOR_BGR2GRAY);
	subs1=prevGray-subs;
	subs2=gray-prevGray;
	adaptiveThreshold( subs1, subs1, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,51,5);
    	dilate(subs1, subs1, kernel);
	multiply(prevGray,subs1,prevGray);
	adaptiveThreshold( subs2, subs2, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,51,5);
    	dilate(subs2, subs2, kernel);
	multiply(gray,subs2,gray);

	double s = cv::sum( gray )[0];
	if(s>=2000){ 
	calcOpticalFlowFarneback(prevGray, gray, flow, 0.5, 2, 21, 2, 7, 1.5, 1);
	image=drawOptFlowMap(flow, image, 15, CV_RGB(0, 255, 0));
	}

	return image;
}
