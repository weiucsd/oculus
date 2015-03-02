#ifndef VPL_HANDS_H
#define VPL_HANDS_H

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include "OvrClass.h"
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

		void DetectHands(const cv::Mat& matInLeft, cv::Mat& matOutLeft, const cv::Mat& matInRight, cv::Mat& matOutRight, VPL::OvrVision& ovrcamera);

		cv::Point3d hand_position_;

		cv::Point3_<int> color_low_ = { 0, 0, 93 };
		cv::Point3_<int> color_high_ = { 21, 80, 173 };

	private:

		cv::Ptr<cv::BackgroundSubtractor> pmog1_;
		cv::Ptr<cv::BackgroundSubtractor> pmog2_;

		cv::Point rough_palm_center_left_;
		cv::Point rough_palm_center_right_;

		float Depth(const cv::Point left, const cv::Point right, VPL::OvrVision& ovrcamera);
		void DetectOneHand(const cv::Mat& matIn, cv::Mat& matOut, const int eye);
		void DetectOneHandMine(const cv::Mat& matIn, cv::Mat& matOut, const int eye);

		std::pair<cv::Point_<double>, double> CircleFromPoints(const cv::Point p1, const cv::Point p2, const cv::Point p3);
		double Dist(const cv::Point x, const cv::Point y);
	};
}
#endif