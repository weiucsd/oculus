#ifndef HD_HANDS_H
#define HD_HANDS_H

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>

namespace VPL {
	class Hands
	{

	public:
		Hands();
		~Hands();

		void DetectHands(const cv::Mat& matIn, cv::Mat& matOut, const int eye);

		cv::Point rough_palm_center;
	private:
		cv::Ptr<cv::BackgroundSubtractor> pmog1_;
		cv::Ptr<cv::BackgroundSubtractor> pmog2_;

		std::pair<cv::Point_<double>, double> CircleFromPoints(const cv::Point p1, const cv::Point p2, const cv::Point p3);
		double Dist(const cv::Point x, const cv::Point y);
	};
}
#endif