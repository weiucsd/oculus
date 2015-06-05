#ifndef VPL_OVRVISION_H
#define VPL_OVRVISION_H

//#include "ovrvision.h"
#include "opencv2/opencv.hpp"

namespace VPL {

class OvrVision
{ 
  public:  
    OvrVision();  
    ~OvrVision();

	void Initialize();
	void GetFrame(cv::Mat& left, cv::Mat& right);
	void Terminate();
	float GetFocalPoint();
	float GetBaseline();

  private:
	 // OVR::Ovrvision* g_pOvrvision_;
};

}  // namespace VPL

#endif