#include "OvrClass.h"
#include "ovrvision.h"

namespace VPL {

	OvrVision::OvrVision()
	{

	}

	OvrVision::~OvrVision()
	{

	}

	void OvrVision::Initialize()
	{
		g_pOvrvision_ = new OVR::Ovrvision();
		g_pOvrvision_->Open(0, OVR::OV_CAMVGA_FULL);	//Open
	}

	float OvrVision::GetFocalPoint()
	{
		return g_pOvrvision_->GetFocalPoint();
	}

	float OvrVision::GetBaseline()
	{
		return g_pOvrvision_->GetOculusRightGap(2);
	}

	void OvrVision::GetFrame(cv::Mat& left, cv::Mat& right)
	{
		int row, col, color;
		int processer_quality = OVR::OV_PSQT_HIGH;

		// Get ovrvision image
		g_pOvrvision_->PreStoreCamData();	//renderer
		unsigned char* p = g_pOvrvision_->GetCamImage(OVR::OV_CAMEYE_LEFT, (OVR::OvPSQuality)processer_quality);
		unsigned char* p2 = g_pOvrvision_->GetCamImage(OVR::OV_CAMEYE_RIGHT, (OVR::OvPSQuality)processer_quality);

		for (col = 0; col < 640; col++)
		{
			for (row = 0; row < 480; row++)
			{
				for (color = 0; color < 3; color++)
				{
					left.at<cv::Vec3b>(row, col)[color] = p[color + col * 3 + row * 640 * 3];
					right.at<cv::Vec3b>(row, col)[color] = p2[color + col * 3 + row * 640 * 3];
				}
			}
		}
	}

	void OvrVision::Terminate()
	{
		delete g_pOvrvision_;
	}
}  // namespace VPL