#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
//#include "opencv2/core/core.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include <unistd.h>

using namespace cv;
using namespace std;

int  main(){

	VideoCapture cap(0);
	Mat frame;		
	cap >> frame;
	imwrite("frame.png",frame);
	imshow("",frame);
	waitKey();

	return 0.0;

}
