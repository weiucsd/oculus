#include "hands.h"
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <math.h>

namespace VPL
{
	Hands::Hands()
	{
		pmog1_ = cv::createBackgroundSubtractorMOG2();
		pmog2_ = cv::createBackgroundSubtractorMOG2();

		kalman_filter_ = cv::KalmanFilter(6, 3, 0, CV_32F);
		kalman_filter_.transitionMatrix = (cv::Mat_<float>(6, 6) << 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1);
		//cv::setIdentity(kalman_filter_.transitionMatrix);

		kalman_filter_.statePre.at<float>(0,0) = 0;
		kalman_filter_.statePre.at<float>(1,0) = 0;
		kalman_filter_.statePre.at<float>(2,0) = 0;
		kalman_filter_.statePre.at<float>(3, 0) = 0;
		kalman_filter_.statePre.at<float>(4, 0) = 0;
		kalman_filter_.statePre.at<float>(5, 0) = 0;
		cv::setIdentity(kalman_filter_.measurementMatrix);
		cv::setIdentity(kalman_filter_.processNoiseCov, cv::Scalar::all(1e-1));
		cv::setIdentity(kalman_filter_.measurementNoiseCov, cv::Scalar::all(10));
		cv::setIdentity(kalman_filter_.errorCovPost, cv::Scalar::all(.1));
	}

	Hands::~Hands()
	{

	}

	std::pair<cv::Point_<double>, double> Hands::CircleFromPoints(const cv::Point p1, const cv::Point p2, const cv::Point p3)
	{
		double offset = std::pow(p2.x, 2) + std::pow(p2.y, 2);
		double bc = (std::pow(p1.x, 2) + std::pow(p1.y, 2) - offset) / 2.0;
		double cd = (offset - std::pow(p3.x, 2) - std::pow(p3.y, 2)) / 2.0;
		double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);
		double TOL = 0.0000001;
		if (abs(det) < TOL)
		{
			std::cout << "POINTS TOO CLOSE" << std::endl;
			return std::make_pair(cv::Point_<double>((double)0, (double)0), (double)0);
		}

		double idet = 1 / det;
		double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
		double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
		double radius = std::sqrt(std::pow(p2.x - centerx, 2) + std::pow(p2.y - centery, 2));

		return std::make_pair(cv::Point_<double>(centerx, centery), radius);
	}

	double Hands::Dist(const cv::Point x, const cv::Point y)
	{
		return (x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y);
	}

	void Hands::DetectHands(const cv::Mat& matInLeft, cv::Mat& matOutLeft, const cv::Mat& matInRight, cv::Mat& matOutRight, VPL::OvrVision& ovrcamera)
	{
		float depth;

		int eye;
		for (eye = 1; eye <= 2; eye++)
		{
			if (eye == 1)
			{
				DetectOneHandMine(matInLeft, matOutLeft, 1);
			}
			else
			{
				DetectOneHandMine(matInRight, matOutRight, 2);
			}
		}

		depth = Depth(rough_palm_center_left_, rough_palm_center_right_, ovrcamera);
		hand_position_.z = depth;
		hand_position_.y = (rough_palm_center_left_.y-640/2)*depth/ovrcamera.GetFocalPoint();
		hand_position_.x = (rough_palm_center_left_.x-480/2)*depth/ovrcamera.GetFocalPoint();

		KalmanFilterHand();
	}

	void Hands::KalmanFilterHand()
	{
		cv::Mat prediction = kalman_filter_.predict();
		cv::Mat measurement(3, 1, CV_32F);
		measurement.at<float>(0,0) = hand_position_.x;
		measurement.at<float>(1,0) = hand_position_.y;
		measurement.at<float>(2,0) = hand_position_.z;

		cv::Mat estimated = kalman_filter_.correct(measurement);
		hand_position_.x = estimated.at<float>(0,0);
		hand_position_.y = estimated.at<float>(1,0);
		hand_position_.z = estimated.at<float>(2,0);
	}

	void Hands::DetectOneHand(const cv::Mat& matIn, cv::Mat& matOut, const int eye)
	{
		unsigned int i, j, ii;
		cv::Mat frame;
		cv::Mat back;
		cv::Mat fore;
		cv::Mat fore1;
		cv::Point rough_palm_center;

		std::vector<std::pair<cv::Point, double> > palm_centers;

		matIn.copyTo(frame);

		std::vector<std::vector<cv::Point> > contours;

		//Filter colors
		cv::inRange(frame, cv::Scalar(color_low_.x, color_low_.y, color_low_.z), cv::Scalar(color_high_.x, color_high_.y, color_high_.z), fore);

		//Update the current background model and get the foreground
		if (eye == 1)
		{
			//pmog1_->apply(frame, fore);
		}
		else
		{
			//pmog2_->apply(frame, fore);
		}

		//cv::bitwise_and(fore, fore1, fore);


		//Enhance edges in the foreground by applying erosion and dilation
		//cv::erode(fore, fore, cv::Mat(), cv::Point(-1, -1), 1);
		//cv::dilate(fore, fore, cv::Mat(), cv::Point(-1, -1), 1);

		//cv::namedWindow("contours");

		//Find the contours in the foreground
		cv::findContours(fore, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (i = 0; i < contours.size(); i++)
			//Ignore all small insignificant areas
			if (cv::contourArea(contours[i]) >= 5000)
			{
				//Draw contour
				std::vector<std::vector<cv::Point> > tcontours;
				tcontours.push_back(contours[i]);
				//drawContours(frame, tcontours, -1, cv::Scalar(0, 0, 255), 2);

				//Detect Hull in current contour
				std::vector<std::vector<cv::Point> > hulls(1);
				std::vector<std::vector<int> > hullsI(1);
				cv::convexHull(cv::Mat(tcontours[0]), hulls[0], false);
				cv::convexHull(cv::Mat(tcontours[0]), hullsI[0], false);
				//drawContours(frame, hulls, -1, cv::Scalar(0, 255, 0), 2);

				//Find minimum area rectangle to enclose hand
				cv::RotatedRect rect = cv::minAreaRect(cv::Mat(tcontours[0]));

				//Find Convex Defects
				std::vector<cv::Vec4i> defects;
				if (hullsI[0].size() > 0)
				{
					cv::Point2f rect_points[4]; rect.points(rect_points);
					//for (j = 0; j < 4; j++)
					//line(frame, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(255, 0, 0), 1, 8);

					//cv::imshow("contours", frame);
					//cv::Point rough_palm_center;
					convexityDefects(tcontours[0], hullsI[0], defects);
					if (defects.size() >= 3)
					{
						std::vector<cv::Point> palm_points;
						for (j = 0; j < defects.size(); j++)
						{
							int startidx = defects[j][0]; cv::Point ptStart(tcontours[0][startidx]);
							int endidx = defects[j][1]; cv::Point ptEnd(tcontours[0][endidx]);
							int faridx = defects[j][2]; cv::Point ptFar(tcontours[0][faridx]);
							//Sum up all the hull and defect points to compute average
							rough_palm_center += ptFar + ptStart + ptEnd;
							palm_points.push_back(ptFar);
							palm_points.push_back(ptStart);
							palm_points.push_back(ptEnd);
						}

						//Get palm center by 1st getting the average of all defect points, this is the rough palm center,
						//Then U chose the closest 3 points ang get the circle radius and center formed from them which is the palm center.
						rough_palm_center.x /= defects.size() * 3;
						rough_palm_center.y /= defects.size() * 3;
						//cv::Point closest_pt = palm_points[0];
						std::vector<std::pair<double, int> > distvec;
						for (ii = 0; ii < palm_points.size(); ii++)
							distvec.push_back(std::make_pair(Dist(rough_palm_center, palm_points[ii]), ii));
						std::sort(distvec.begin(), distvec.end());

						//Keep choosing 3 points till you find a circle with a valid radius
						//As there is a high chance that the closes points might be in a linear line or too close that it forms a very large circle
						std::pair<cv::Point, double> soln_circle;
						for (ii = 0; ii + 2 < distvec.size(); ii++)
						{
							cv::Point p1 = palm_points[distvec[ii + 0].second];
							cv::Point p2 = palm_points[distvec[ii + 1].second];
							cv::Point p3 = palm_points[distvec[ii + 2].second];
							soln_circle = CircleFromPoints(p1, p2, p3);//Final palm center,radius
							if (soln_circle.second != 0)
								break;
						}

						//Find avg palm centers for the last few frames to stabilize its centers, also find the avg radius
						palm_centers.push_back(soln_circle);
						if (palm_centers.size() > 10)
							palm_centers.erase(palm_centers.begin());

						cv::Point palm_center;
						double radius = 0;
						for (ii = 0; ii < palm_centers.size(); ii++)
						{
							palm_center += palm_centers[ii].first;
							radius += palm_centers[ii].second;
						}
						palm_center.x /= palm_centers.size();
						palm_center.y /= palm_centers.size();
						radius /= palm_centers.size();

						//Draw the palm center and the palm circle
						//The size of the palm gives the depth of the hand
						circle(frame, rough_palm_center, (int)5, cv::Scalar(144, 144, 255), (int)3);
						circle(frame, rough_palm_center, (int)radius, cv::Scalar(144, 144, 255), (int)2);
					}

				}
			}

		//frame.copyTo(matOut);
		frame.copyTo(matOut);

		if (eye == 1)
		{
			rough_palm_center_left_ = rough_palm_center;
		}
		else
		{
			rough_palm_center_right_ = rough_palm_center;
		}
	}

	float Hands::Depth(const cv::Point left, const cv::Point right,VPL::OvrVision& ovrcamera)
	{

		float focal_point,baseline;

		cv::Point disparity;
		disparity = rough_palm_center_left_ - rough_palm_center_right_;

		focal_point = ovrcamera.GetFocalPoint();
		baseline = ovrcamera.GetBaseline();

		return (baseline*focal_point) / (float)disparity.x;
	}

	void Hands::DetectOneHandMine(const cv::Mat& matIn, cv::Mat& matOut, const int eye)
	{
		int i, j;
		cv::Mat frame;
		cv::Mat back;
		cv::Mat fore;
		cv::Mat fore1;
		cv::Point rough_palm_center;

		matIn.copyTo(frame);


		//Filter colors
		cv::inRange(frame, cv::Scalar(color_low_.x, color_low_.y, color_low_.z), cv::Scalar(color_high_.x, color_high_.y, color_high_.z), fore);

		int counter = 0;
		cv::Mat points_row(frame.rows,frame.cols,CV_64FC1);
		cv::Mat points_col(frame.rows, frame.cols, CV_64FC1);

		for (i = 0; i < fore.rows; i++)
		{
			for (j = 0; j < fore.cols; j++)
			{
				points_row.at<double>(i, j) = (float) i;
				points_col.at<double>(i, j) = (float) j;
			}
		}

		rough_palm_center.y = cv::mean(points_row,fore)[0];
		rough_palm_center.x = cv::mean(points_col,fore)[0];


		if (eye == 1)
		{
			rough_palm_center_left_ = rough_palm_center;
		}
		else
		{
			rough_palm_center_right_ = rough_palm_center;
		}

		circle(frame, rough_palm_center, (int)20, cv::Scalar(144, 144, 255), (int)2);
		frame.copyTo(matOut);
	}
}