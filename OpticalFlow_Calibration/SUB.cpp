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
	
	//VideoCapture cap(0);
	double C, VelF;
	VelF=0;
	Mat gray, prevGray, image, frame, subs;
	namedWindow( "Algorythm", 0 );	
	
	subs=imread("y/frame0.png");
	prevGray=imread("y/frame2.png");
	gray=imread("y/frame4.png");

	image=Subtraction(subs, prevGray, gray, subs);
	
	imwrite("imageSUB.png",image);

	return 0.0;
}
