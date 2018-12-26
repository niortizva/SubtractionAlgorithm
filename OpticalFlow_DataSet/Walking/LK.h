/*This library was created by Nicolas Ortiz and the research group on Image Processing of the 
Universidad Piloto de Colombia.  
*/

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
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

/* Lucas Kanade Optical Flow mixed with the subtraction Algorythm
*/

Mat LK(Mat subs, Mat prevGray, Mat gray, Mat image){

	Mat subs1, subs2, kernel;
	kernel = Mat::ones(51, 51, CV_8UC1); 
	TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,100,0.03);
	Size subPixWinSize(7,7), winSize(31,31);
	Point2f point;	
	const int MAX_COUNT=1000;
	double min, max;
	vector<Point2f> points[2];

	subs=RGBtoXYZ(subs);
	cvtColor(subs, subs, COLOR_BGR2GRAY);
	prevGray=RGBtoXYZ(prevGray);
	cvtColor(prevGray, prevGray, COLOR_BGR2GRAY);
	gray=RGBtoXYZ(gray);
	cvtColor(gray, gray, COLOR_BGR2GRAY);
	subs1=prevGray-subs;
	subs2=gray-prevGray;
	adaptiveThreshold( subs1, subs1, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,81,5);
    	dilate(subs1, subs1, kernel);
	multiply(prevGray,subs1,prevGray);
	minMaxLoc(prevGray, &min, &max);
	double s = cv::sum( prevGray )[0];
	if(s>=2000){
	//fast.detect(prevGray,points[0],kernel);
	goodFeaturesToTrack(prevGray, points[0], MAX_COUNT, 0.01, 1, Mat(), 3, 0, 0.04);
	cornerSubPix(prevGray, points[0], subPixWinSize, Size(-1,-1), termcrit);
	adaptiveThreshold( subs2, subs2, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,81,5);
    	dilate(subs2, subs2, kernel);
	multiply(gray,subs2,gray);
	minMaxLoc(gray,&min,&max);
	double s = cv::sum( gray )[0];
	if(s>=2000){ 
	//fast.detect(prevGray,points[1],kernel);
	goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 1, Mat(), 3, 0, 0.04);
	cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
	vector<uchar> status;
        vector<float> err;
	calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,3, termcrit, 0, 0.01); 
	size_t i, k;
	double VM, Phi, X1, X0, Y1, Y0, X, Y, L;
	VM=0; Phi=0; X=0; Y=0;
		
        for( i = k = 0; i < points[1].size(); i++ ){

		points[1][k++] = points[1][i];
              	circle( image, points[1][i], 3, Scalar(0,0,0), -1, 8);
		X1=points[1][i].x; Y1=points[1][i].y;
		X0=points[0][i].x; Y0=points[0][i].y;
		CvPoint p0 =cvPoint(cvRound(points[0][i].x),cvRound(points[0][i].y));
		CvPoint p1 =cvPoint(cvRound(points[1][i].x),cvRound(points[1][i].y));
		if(sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0))<25){
				VM=VM + sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
				if(X1!=X0){
				Phi=Phi+atan((Y1-Y0)/(X1-X0));
				}
				else if(Y1-Y0>0){
				Phi=3.1415/2;
				}
				else if(Y1-Y0<0){
				Phi=-3.1415/2;
				}
				else if(Y1-Y0==0){
				Phi=0;
				}	
				X=X+X1; Y=Y+Y1;
				line(image,p0,p1,CV_RGB(0,255,0),0.1,8);
		}
	}

	VM=VM/points[1].size(); Phi=Phi/points[1].size();
	X=X/points[1].size(); Y=Y/points[1].size(); 
	double Xnew=X+VM*cos(Phi); double Ynew=Y+VM*sin(Phi);	
	CvPoint p0 =cvPoint(cvRound(X),cvRound(Y));
	CvPoint p1 =cvPoint(cvRound(Xnew),cvRound(Ynew));
	circle( image, p0, 8, Scalar(255,0,0), -1, 8);
	circle( image, p1, 8, Scalar(0,0,255), -1, 8);
	std::ostringstream strs;
	strs << VM;
	std::string str = strs.str();
	std::ostringstream strs2;
	strs2 << Phi;
	std::string str2 = strs2.str();
	putText(image, "Magnitude: "+str, Point2f(X,Y),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);
	putText(image, "Angle: "+str2, Point2f(X,Y-30),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);
	line(image,p0,p1,CV_RGB(0,0,0),5,8);
	
	points[1].resize(k);
	if( points[1].size() < (size_t)MAX_COUNT ){
			
		vector<Point2f> tmp;
		tmp.push_back(point);
        	cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            	points[0].push_back(tmp[1]);	   	

        }
	}
	}

	return image;	
	
}
