#include <iostream>
#include "walala.h"
#include "ovrvision.h"
#include "OVR.h"
#include "hands.h"
#include "OculusBase.h"
#include "OculusSampleScene.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Not enough arguments" << std::endl;
		return -1;
	}

	VPL::OculusBase oculusbase;
	VPL::OculusSampleScene oculusscene;
	oculusbase.InitRendering(oculusscene);

	VPL::Hands hands;

	cv::Mat mat;
	cv::Mat mat2;

	cv::Mat mat_hands;
	cv::Mat mat_hands2;

	cv::namedWindow("left");
	cv::namedWindow("right");

	for (int ii = 0; ii < 125; ii++)
	{
		char filename[300];
		sprintf_s(filename, 300, "%s\\left\\file%d.png", argv[1], ii);
		mat = cv::imread(filename);

		sprintf_s(filename, 300, "%s\\right\\file%d.png", argv[1], ii);
		mat2 = cv::imread(filename);

		hands.DetectHands(mat, mat_hands, 1);
		hands.DetectHands(mat2, mat_hands2, 2);

		imshow("left", mat_hands);
		imshow("right", mat_hands2);
		cv::waitKey(10);

		oculusscene.x_ = hands.rough_palm_center.x/200;
		oculusscene.y_ = hands.rough_palm_center.y/200;

		oculusbase.RenderFrame(oculusscene);
		cv::waitKey(10);
	}

	oculusbase.RenderTerminate();
	return 0;
}
