#include <iostream>
#include "walala.h"

namespace VPL {

walala::walala(){}

walala::~walala(){}

void walala::TestPrint()
{
	std::cout << "you passed the test!" << std::endl;
}

void walala::TestShowImage(const char *img_name)
{
	cv::namedWindow( "test window", cv::WINDOW_AUTOSIZE );

	cv::imshow("test window", cv::imread(img_name));

	cv::waitKey(0);
}

}  // namespace VPL