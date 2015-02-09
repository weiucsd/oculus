#include <iostream>
#include "walala.h"
#include "ovrvision.h"
#include "OVR.h"

int main(int argc, char *argv[])
{
	ovrHmd           HMD;
	OVR::Ovrvision* g_pOvrvision;
	g_pOvrvision = new OVR::Ovrvision();
	/*
	if(argc!=2)
	{
		std::cout << "please append the input path to your image behind ./walalaExample" << std::endl;
		return -1;
	}

	VPL::walala walalaobj;

	walalaobj.TestShowImage(argv[1]); 
	walalaobj.TestPrint(); 
	*/
	cv::namedWindow("test window", cv::WINDOW_AUTOSIZE);
	cv::imshow("test window", cv::imread(argv[1]));
	cv::waitKey(0);

	return 0;
}
