#include <iostream>
#include "walala.h"
#include "OvrClass.h"
#include "OVR.h"
#include "hands.h"
#include "OculusBase.h"
#include "OculusSampleScene.h"
#include <sstream>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Not enough arguments" << std::endl;
		return -1;
	}

	std::istringstream ss(argv[2]);
	int flag;
	ss >> flag;

	VPL::OculusBase oculusbase;
	VPL::OculusSampleScene oculusscene;
	oculusbase.InitRendering(oculusscene);

	VPL::OvrVision ovrcamera;
	ovrcamera.Initialize();

	VPL::Hands hands;

	cv::Mat mat(480,640,CV_8UC3);
	cv::Mat mat2(480, 640, CV_8UC3);

	cv::Mat mat_hands;
	cv::Mat mat_hands2;

	VPL::OvrVision ovr;
	ovr.Initialize();

	cv::namedWindow("left hands");
	cv::namedWindow("right hands");

	int x = 2000;
	int y = 200;
	cv::moveWindow("left hands", x, y);
	cv::moveWindow("right hands", x+1000, y);

	if (flag)
	{
		while (1)
		{
			//Get frame from RGB cameras
			ovrcamera.GetFrame(mat, mat2);
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
			cv::cvtColor(mat2, mat2, cv::COLOR_BGR2RGB);

			//Run hand detection algorithm
			hands.DetectHands(mat, mat_hands, mat2, mat_hands2, ovr);

			//Show location of hand in RGB
			imshow("left hands", mat_hands);
			imshow("right hands", mat_hands2);
			cv::waitKey(10);

			//Update location of cube using location of hand
			oculusbase.UpdateScene(oculusscene, hands);
			oculusbase.RenderFrame(oculusscene);
			cv::waitKey(10);
		}
	}
	else
	{
		for (int ii = 0; ii < 125; ii++)
		{
			char filename[300];
			sprintf_s(filename, 300, "%s\\left\\file%d.png", argv[1], ii);
			mat = cv::imread(filename);

			sprintf_s(filename, 300, "%s\\right\\file%d.png", argv[1], ii);
			mat2 = cv::imread(filename);

			//Run hand detection algorithm
			hands.DetectHands(mat, mat_hands, mat2, mat_hands2, ovr);

			//Show location of hand in RGB
			imshow("left hands", mat_hands);
			imshow("right hands", mat_hands2);
			cv::waitKey(10);

			//Update location of cube using location of hand
			oculusbase.UpdateScene(oculusscene, hands);
			oculusbase.RenderFrame(oculusscene);
			cv::waitKey(10);
		}

	}

	oculusbase.RenderTerminate();
	return 0;
}
