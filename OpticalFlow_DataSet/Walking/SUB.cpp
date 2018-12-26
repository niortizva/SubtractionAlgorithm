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
	
	subs=imread("frame12.png");
	prevGray=imread("frame13.png");
	gray=imread("frame14.png");

	image=Subtraction(subs, prevGray, gray, subs);
	
	imwrite("imageSUB.png",image);

	return 0.0;

}
