#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <vector>
#include <stdio.h>

using namespace cv;
using namespace std;

class Hands
{
	BackgroundSubtractorMOG2 bg1;
	BackgroundSubtractorMOG2 bg2;

	int nmixtures;
	bool detectShadows;

	int backgroundFrame;
	int backgroundFrame2;

	int iter;
public:
	Hands(int frame, int frame2, int m, int d);
	~Hands();

	void DetectHands(Mat& matIn, Mat& matOut, int eye);
	pair<Point, double> circleFromPoints(Point p1, Point p2, Point p3);
	double dist(Point x, Point y);

private:
};