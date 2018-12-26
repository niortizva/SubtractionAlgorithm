/*This library was created by Nicolas Ortiz and the research group on Image Processing of the 
Universidad Piloto de Colombia.  
*/

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
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

/*Phase Correlation Algorithm
*/

Mat PC(Mat subs, Mat prevGray, Mat gray, Mat image){

	Mat kernel= Mat::ones(51, 51, CV_8UC1);
	Mat subs1, subs2;
	Mat curr64f, prev64f, hann;
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

	createHanningWindow(hann, gray.size(), CV_64F);

	prevGray.convertTo(prev64f, CV_64F);
        gray.convertTo(curr64f, CV_64F);

	Point2d shift = phaseCorrelate(prev64f, curr64f, hann);
        double radius = std::sqrt(shift.x*shift.x + shift.y*shift.y);

	Point center(gray.cols >> 1, gray.rows >> 1);
        circle(image, center, (int)radius, Scalar(0, 255, 0), 3, LINE_AA);
        line(image, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), Scalar(0, 255, 0), 3, LINE_AA);

	double VM=sqrt((int)shift.x*(int)shift.x+(int)shift.y*(int)shift.y);
	double Phi;
	if((int)shift.x!=0){
		Phi=atan((int)shift.y/(int)shift.x);
	}
	else if((int)shift.y>0){
		Phi=3.1415/2;	
	}
	else if((int)shift.y<0){
		Phi=-3.1415/2;	
	}
	else if((int)shift.y==0){
		Phi=0;	
	}
	std::ostringstream strs;
	strs << VM;
	std::string str = strs.str();
	std::ostringstream strs2;
	strs2 << Phi;
	std::string str2 = strs2.str();
	putText(image, "Magnitude: "+str, Point(center.x + (int)shift.x, center.y + (int)shift.y),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);
	putText(image, "Angle: "+str2, Point(center.x + (int)shift.x, center.y + (int)shift.y-30),FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0,0), 2);

	}
	
	return image;

}
