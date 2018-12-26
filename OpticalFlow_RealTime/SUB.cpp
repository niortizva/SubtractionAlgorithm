#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include "SUB.h"

using namespace cv;
using namespace std;

int  main(){
	
	VideoCapture cap(0);
	double C, VelF;
	VelF=0;
	Mat gray, prevGray, image, frame, subs;
	namedWindow( "Algorythm", 0 );

	cap >> frame;
	frame.copyTo(subs);
	cap >> frame;
	frame.copyTo(prevGray);	

	while(1){

		cap >> frame;
		frame.copyTo(gray);
		
		image=Subtraction(subs, prevGray, gray, subs);
		imshow("Algorythm",image);
		
		prevGray.copyTo(subs);
		gray.copyTo(prevGray);
			
		C= cvWaitKey(11);
    		if (C == 27) break;		
		
	}
	return 0.0;
}
