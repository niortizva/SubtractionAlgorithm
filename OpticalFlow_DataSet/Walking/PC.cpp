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
#include "PC.h"

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

	image=PC(subs, prevGray, gray, subs);
	
	imwrite("imagePC.png",image);

	return 0.0;

}
