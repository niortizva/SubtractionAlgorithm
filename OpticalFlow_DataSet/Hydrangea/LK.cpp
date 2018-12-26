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
	
	subs=imread("frame10.png");
	prevGray=imread("frame11.png");
	gray=imread("frame12.png");

	image=LK(subs, prevGray, gray, subs);
	
	imwrite("imageLK.png",image);

	return 0.0;

}
