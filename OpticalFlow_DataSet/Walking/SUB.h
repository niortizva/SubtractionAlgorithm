#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
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

Mat Subtraction(Mat actu, Mat prev, Mat subs, Mat frame){

	Mat subs1, subs2;
	Mat kernel= Mat::ones(3, 3, CV_8UC1);
	imwrite("frame.png", subs);
	cvtColor(subs,subs,COLOR_RGB2GRAY);
	cvtColor(prev,prev,COLOR_RGB2GRAY);
	cvtColor(actu,actu,COLOR_RGB2GRAY);
	subs1=prev-subs;
	subs2=actu-prev;
	adaptiveThreshold( subs1, subs1, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,51,5);
	dilate(subs1, subs1, kernel);
	multiply(prev,subs1,prev);
	adaptiveThreshold( subs2, subs2, 1, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,51,5);
	dilate(subs2, subs2, kernel);
	multiply(actu,subs2,actu);
	//fastNlMeansDenoising(prev,prev,3,17,21);
	//fastNlMeansDenoising(actu,actu,3,17,21);
	double posx, posy, posxo, posyo; int i=0; double h,l; int Yo, Yf, Xo, Xf;
	posxo=0; posyo=0; posx=0; posy=0; int x,y; 

	for (y = 0; y < prev.rows; ++y) {
    		for (x = 0; x < prev.cols; ++x) {
			if(prev.at<uchar>(y,x)>=10){
				posxo += x; posyo +=y;
				i += 1;
			}
		}	
	}

	posxo=int(posxo/i); posyo=int(posyo/i);
	i=0; 

	for (y = 0; y < actu.rows; ++y) {
    		for (x = 0; x < actu.cols; ++x) {
			if(actu.at<uchar>(y,x)>=10){
				posx += x; posy +=y;
				if(i==0){
					Xo=x;Yo=y;
				} 
				i += 1; Yf=y; Xf=x;
			}
		}	
	}

	posx=int(posx/i); posy=int(posy/i);

	//cout << int(posx-posxo) << setw(10) << int(posy-posyo) << setw(10) << i << endl;
	//cout << posx << setw(10) << posy << endl;
	//cout << int(posxo) << setw(10) << int(posyo) << endl;

	rectangle(frame,cvPoint(Xo,Yo),cvPoint(Xf,Yf),Scalar(255,0,0),2,8,0);
	line(frame,cvPoint(posxo,posyo),cvPoint(posx,posy),Scalar(0,255,0),2,8,0);

	double VM=sqrt((posxo-posx)*(posxo-posx)+(posyo-posy)*(posyo-posy));
	double Phi;

	if(posxo-posx!=0){
		Phi=atan((posy-posyo)/(posx-posxo));	
	}
	else if(posy-posyo>0){
		Phi=3.1415/2;
	}
	else if(posy-posyo<0){
		Phi=-3.1415/2;
	}

	std::ostringstream strs;
	strs << VM;
	std::string str = strs.str();
	std::ostringstream strs2;
	strs2 << Phi;
	std::string str2 = strs2.str();
	putText(frame, "Magnitude: "+str, Point(posx,posy),FONT_HERSHEY_PLAIN, 2,  CV_RGB(255,0,0), 2);
	putText(frame, "Angle: "+str2, Point(posx,posy-30),FONT_HERSHEY_PLAIN, 2,  CV_RGB(255,0,0), 2);

	return frame;

}
