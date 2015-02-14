#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

class Hands
{
	
public:
	Hands(const int frame, const int frame2, const int m, const bool d);
	~Hands();

	void DetectHands(const cv::Mat& matIn, cv::Mat& matOut,const int eye);
private:
	cv::BackgroundSubtractorMOG2 bg1;
	cv::BackgroundSubtractorMOG2 bg2;

	int nmixtures;
	bool detectShadows;

	int backgroundFrame;
	int backgroundFrame2;

	int iter;

	std::pair<cv::Point_<double>, double> circleFromPoints(const cv::Point p1, const cv::Point p2, const cv::Point p3);
	double dist(const cv::Point x, const cv::Point y);
};