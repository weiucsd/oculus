#include <iostream>
#include "walala.h"
#include "ovrvision.h"
#include "OVR.h"
#include "hands.h"

using namespace cv;

int main(int argc, char *argv[])
{
	ovrHmd           HMD;
	OVR::Ovrvision* g_pOvrvision;
	g_pOvrvision = new OVR::Ovrvision();
	
	Hands hands(500,500,10,false);

	Mat mat;
	Mat mat2;

	Mat matHands;
	Mat matHands2;

	namedWindow("left", 1);
	namedWindow("right", 1);

	for (int ii = 0; ii < 202; ii++)
	{
		char filename[300];
		sprintf(filename, "D:\\Documents\\VR-Hands-VPL\\Testing\\left\\file%d.png", ii);
		mat = cv::imread(filename);

		sprintf(filename, "D:\\Documents\\VR-Hands-VPL\\Testing\\right\\file%d.png", ii);
		mat2 = cv::imread(filename);
		
		hands.DetectHands(mat, matHands, 1);
		hands.DetectHands(mat2, matHands2, 2);

		imshow("left", matHands);
		imshow("right", matHands2);
		waitKey(10);
	}

	return 0;
}
