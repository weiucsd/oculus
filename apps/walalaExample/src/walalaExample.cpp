#include <iostream>
#include "walala.h"
#include "ovrvision.h"
#include "OVR.h"
#include "hands.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Not enough arguments" << std::endl;
		return -1;
	}

	ovrHmd hmd;
	OVR::Ovrvision* g_povrvision;
	g_povrvision = new OVR::Ovrvision();
	HD::Hands hands;

	cv::Mat mat;
	cv::Mat mat2;

	cv::Mat mat_hands;
	cv::Mat mat_hands2;

	cv::namedWindow("left", 1);
	cv::namedWindow("right", 1);

	for (int ii = 0; ii < 202; ii++)
	{
		char filename[300];
		char extension[300];
		sprintf_s(filename, "%s", argv[1]);
		sprintf_s(extension, "\\left\\file%d.png", ii);
		strcat_s(filename, extension);
		mat = cv::imread(filename);

		sprintf_s(filename, "%s", argv[1]);
		sprintf_s(extension, "\\right\\file%d.png", ii);
		strcat_s(filename, extension);
		mat2 = cv::imread(filename);
		
		hands.DetectHands(mat, mat_hands, 1);
		hands.DetectHands(mat2, mat_hands2, 2);

		imshow("left", mat_hands);
		imshow("right", mat_hands2);
		cv::waitKey(1);
	}

	return 0;
}
