#include "hands.h"
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <math.h>

HD::Hands::Hands()
{
	pmog1_ = cv::createBackgroundSubtractorMOG2();
	pmog2_ = cv::createBackgroundSubtractorMOG2();
}

HD::Hands::~Hands()
{}

std::pair<cv::Point_<double>, double> HD::Hands::CircleFromPoints(const cv::Point p1, const cv::Point p2, const cv::Point p3)
{
	double offset = pow(p2.x, 2) + pow(p2.y, 2);
	double bc = (pow(p1.x, 2) + pow(p1.y, 2) - offset) / 2.0;
	double cd = (offset - pow(p3.x, 2) - pow(p3.y, 2)) / 2.0;
	double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);
	double TOL = 0.0000001;
	if (abs(det) < TOL) 
	{ 
		std::cout << "POINTS TOO CLOSE" << std::endl; 
		return std::make_pair(cv::Point_<double>((double) 0, (double) 0), (double) 0); 
	}

	double idet = 1 / det;
	double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
	double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
	double radius = sqrt(pow(p2.x - centerx, 2) + pow(p2.y - centery, 2));

	return std::make_pair(cv::Point_<double>(centerx, centery), radius);
}

double HD::Hands::Dist(const cv::Point x, const cv::Point y)
{
	return (x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y);
}

void HD::Hands::DetectHands(const cv::Mat& matIn, cv:: Mat& matOut, const int eye)
{
	unsigned int i,j,ii;
	cv::Mat frame;
	cv::Mat back;
	cv::Mat fore;

	std::vector<std::pair<cv::Point, double> > palm_centers;

	matIn.copyTo(frame);

	std::vector<std::vector<cv::Point> > contours;

	//Update the current background model and get the foreground
	if (eye == 1)
	{
		pmog1_->apply(frame, fore);
	}
	else
	{
		pmog2_->apply(frame, fore);
	}

	//Enhance edges in the foreground by applying erosion and dilation
	cv::erode(fore, fore, cv::Mat());
	cv::dilate(fore, fore, cv::Mat());


	//Find the contours in the foreground
	findContours(fore, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	for (i = 0; i < contours.size(); i++)
		//Ignore all small insignificant areas
		if (cv::contourArea(contours[i]) >= 5000)
		{
			//Draw contour
			std::vector<std::vector<cv::Point> > tcontours;
			tcontours.push_back(contours[i]);
			drawContours(frame, tcontours, -1, cv::Scalar(0, 0, 255), 2);

			//Detect Hull in current contour
			std::vector<std::vector<cv::Point> > hulls(1);
			std::vector<std::vector<int> > hullsI(1);
			cv::convexHull(cv::Mat(tcontours[0]), hulls[0], false);
			cv::convexHull(cv::Mat(tcontours[0]), hullsI[0], false);
			drawContours(frame, hulls, -1, cv::Scalar(0, 255, 0), 2);

			//Find minimum area rectangle to enclose hand
			cv::RotatedRect rect = cv::minAreaRect(cv::Mat(tcontours[0]));

			//Find Convex Defects
			std::vector<cv::Vec4i> defects;
			if (hullsI[0].size() > 0)
			{
				cv::Point2f rect_points[4]; rect.points(rect_points);
				for (j = 0; j < 4; j++)
					line(frame, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(255, 0, 0), 1, 8);
				cv::Point rough_palm_center;
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
					sort(distvec.begin(), distvec.end());

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
					circle(frame, palm_center, (int) 5, cv::Scalar(144, 144, 255), (int) 3);
					circle(frame, palm_center, (int) radius, cv::Scalar(144, 144, 255), (int) 2);

					//Detect fingers by finding points that form an almost isosceles triangle with certain thesholds
					int no_of_fingers = 0;
					for (j = 0; j < defects.size(); j++)
					{
						int startidx = defects[j][0]; cv::Point ptStart(tcontours[0][startidx]);
						int endidx = defects[j][1]; cv::Point ptEnd(tcontours[0][endidx]);
						int faridx = defects[j][2]; cv::Point ptFar(tcontours[0][faridx]);
						//X o--------------------------o Y
						double Xdist = sqrt(Dist(palm_center, ptFar));
						double Ydist = sqrt(Dist(palm_center, ptStart));
						double length = sqrt(Dist(ptFar, ptStart));

						double retLength = sqrt(Dist(ptEnd, ptFar));
						//Play with these thresholds to improve performance
						if (length <= 3 * radius&&Ydist >= 0.4*radius&&length >= 10 && retLength >= 10 && std::max(length, retLength) / std::min(length, retLength) >= 0.8)
							if (std::min(Xdist, Ydist) / std::max(Xdist, Ydist) <= 0.8)
							{
								if ((Xdist >= 0.1*radius&&Xdist <= 1.3*radius&&Xdist<Ydist) || (Ydist >= 0.1*radius&&Ydist <= 1.3*radius&&Xdist>Ydist))
									line(frame, ptEnd, ptFar, cv::Scalar(0, 255, 0), 1), no_of_fingers++;
							}


					}
				}

			}
		}
	frame.copyTo(matOut);
}