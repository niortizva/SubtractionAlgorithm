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
#include "LK.h"

using namespace cv;
using namespace std;

int  main(){
	
	VideoCapture cap(0);
	double C, VelF;
	VelF=0;
	Mat gray, prevGray, image, frame, subs;
	namedWindow( "Algorythm", 0 );	
	double t;
	double h=1;
	
	ofstream file;
	file.open("LK.txt");
	file.precision(4);
	file.setf(ios::fixed | ios::showpoint);
	cout.precision(4);
	cout.setf(ios::fixed | ios::showpoint);
	
	cap >> frame;
	frame.copyTo(subs);
	cap >> frame;
	frame.copyTo(prevGray);
	cap >> frame;

	while(1){

		t=(double)cvGetTickCount();

		cap >> frame;
		frame.copyTo(gray);

		image=LK(subs, prevGray, gray, subs);
		imshow("Algorythm",image);

		prevGray.copyTo(subs);
		gray.copyTo(prevGray);
		
		t=(double)cvGetTickCount()-t;
		t=t/((double)cvGetTickFrequency()*1000.);
		
		C=cvWaitKey(11);
		if(C==27) break;			
		file << t << setw(24) << 3*h << endl;
		h=h+1;
		if(h==100) break;

	}

	return 0.0;

}
