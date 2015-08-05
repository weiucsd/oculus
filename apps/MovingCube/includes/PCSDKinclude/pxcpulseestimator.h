/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013, 2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include "pxcbase.h"
#include "pxccapture.h"

class PXCPulseEstimator : public PXCBase
{
public:
    PXC_CUID_OVERWRITE(PXC_UID('P','U','L','E'));

	//////////////////////////////////////////////////////////////////////////////////////
	/// PULSE ESTIMATOR CONFIGURATION STRUCTURE 
	/// 
	/// Host application loop:
	/// Capture -> ProcessImageAsync(img,data) --> [on-demand] ProcessWaveformsAsync(data) 
	///
	/// Steps performed in this library:
	///            Waveform Generation --> Rate Estimation (one per EstimatorProfile)
	///
	struct ProfileInfo {
		PXC_DEFINE_CONST(MAX_NUM_ESTIMATE, 20);    // max number of rate estimation, 1 estimate per ROI 

		PXCCapture::VideoStream::DataDesc inputs;  // input stream requests
		pxcU32                      numEstimates;  // number of estimator
		pxcU32                      reserved[24];  // reserved; must be zero
								
		/// Configuration for each estimation
		struct EstimatorProfile {

			PXCImage::ColorFormat   inputformat;   // input of estimation by color format 
											      
			enum ROIMode {
				ROI_USE_FACE_TRACKER,          // Face tracker rectangle as ROI 
				ROI_USE_PROFILE_RECTANGLE,     // Profile rectangle as ROI 
			} roiMode;                         // ROI mode of this estimate, default: ROI_USE_FACE_TRACKER 

			PXCRectU32  rectangle;             // ROI rectangle (x,y,w,h), default: -1,-1,-1,-1 as center of image 
			PXCRangeF32 roiWidthInsetRange;    // left and right margin of sampling ROI relative to face/input rectangle in % 
			PXCRangeF32 roiHeightInsetRange;   // top and bottom margin of sampling ROI relative to face/input rectangle in % 

			pxcU32      fid;                   // face identifier, default: 0 for the biggest face 
			pxcU32      numSamplesPerWaveform; // expected number of samples per waveform

			enum Algorithm {
				ALGORITHM_TIME,                // time domain analysis 
				ALGORITHM_FREQUENCY,           // frequency domain analysis
			} algorithm;                       // analysis selection
		} estimator[MAX_NUM_ESTIMATE];         // array of estimator sub-configurations
	};

	/// Pulse Estimator Data Structure
	class Data : public PXCBase {	   // input-output buffer for the system
	public:
		PXC_DEFINE_CONST(MAX_NUM_ESTIMATE,ProfileInfo::MAX_NUM_ESTIMATE); // max number of rate estimation, 1 estimate per ROI
		PXC_DEFINE_CONST(MAX_NUM_SIGNAL_CHANNEL,8);                       // max number of channels in an image 
		
		/// Region-Of-Interest data structure
		struct RegionOfInterest {
			enum ROIType{
				ROI_TYPE_RECT     = 0x00000001,     // ROI as rectangle 
				ROI_TYPE_MASK     = 0x00000002,     // ROI as bit mask  
				ROI_TYPE_FACE_RECT= 0x00010001,     // ROI as face rectangle
			} type;

			PXCRectU32  rectangle;      // face rectangle 
			pxcU32      fid;			// face identifier 
			pxcU32      confidence;     // [0-100] 
            pxcU32      reserved[24];     
		};

		/// Waveform signals data structure
		struct WaveformGroup {
			pxcU32  numWaveforms;                           // number of waveforms 
			pxcU32  numSamplesPerWaveform;                  // number of samples in each waveform 
			pxcF64* waveform[MAX_NUM_SIGNAL_CHANNEL];       // waveform data 
			pxcU8*  waveformMarker[MAX_NUM_SIGNAL_CHANNEL]; // waveform marker  
		};

		/// Sampling group data structure
		struct SamplingGroup {
			RegionOfInterest*  sourceROI;		// reference source ROI info 
			RegionOfInterest samplingROI;		// sampling ROI of the waveform group 
			pxcU64*           timeStamps;		// time stamps in the unit defined by timerFrequency 
			pxcU64        timerFrequency;		// timer frequency in Hz 
			WaveformGroup        signals;       // collection of waveforms and info 
		}; 

		/// Rate Estimate data structure
		struct RateEstimate {                      
			pxcF64 rate;						// rate estimate in BPM 
			pxcF64 estimationConfidence;		// confidence value of the estimation [0-1]
		};

		/// Estimator output data structure
		struct EstimatorResult {                    
			SamplingGroup*    sourceSamples;	 // source of the waveforms of this estimation 
			WaveformGroup  processedSamples;	 // processed waveforms by estimator algorithm 
			WaveformGroup frequencySpectrum;	 // frequency spectrums by estimator algorithm
			pxcU32        channelOfInterest;	 // best channel detected by estimator algorithm 
			RateEstimate           estimate;	 // output estimate structure 
		} estimatorResult[MAX_NUM_ESTIMATE];	 // array of rate estimations 

		pxcU32       numEstimates;		// number of estimates in estimatorResult specified in Profile 
		RateEstimate masterEstimate;	// Master rate output by averaging all estimates 
	};

	/// Get available configuration(s).
    /// pidx        Zero-based index to enumerate all available configurations
    /// pinfo       The configuration structure, to be returned.
    virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;

    /// Get the current working configuration
    /// pinfo       The configuration structure, to be returned.
    pxcStatus __inline QueryProfile(ProfileInfo *pinfo) { return QueryProfile((pxcU32)WORKING_PROFILE,pinfo); }

    /// Set the working configuration
    /// pinfo       The configuration structure
    virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;

	/// Create a Pulse Estimator data buffer according to internal profile status
	/// data		Buffer for groups of waveforms and rate estimates, to be returned
	virtual pxcStatus PXCAPI CreateData(Data **)=0;

	///********** Pulse Estimator Step 1 **************************/
	/// Forming Waveform the input image(s) for computing Pulse Estimator data
	/// images		The input image arrays 
	/// data		Groups of Waveforms, to be returned
	/// sp   		The sync point, to be returned
    virtual pxcStatus PXCAPI ProcessImageAsync(PXCImage *images[], Data** data, PXCScheduler::SyncPoint **sp)=0;

	///********** Pulse Estimator Step 2 (on-demand) **************/
	/// data 		Groups of Waveforms, including return buffer
	/// output 		Rate Estimate in the data buffer
	virtual pxcStatus PXCAPI ProcessWaveformsAsync( Data* data, PXCScheduler::SyncPoint **sp)=0;
	
	/// Waveform filtering options
	enum WaveformFilterOptions {
		WAVEFORM_FILTER_NORMALIZE   = 0x000001,    /* normalize the waveform into 0.0-1.0 scale  */
		WAVEFORM_FILTER_SMOOTH      = 0x000002,    /* smooth the waveform by a 1x5 linear filter */
		WAVEFORM_FILTER_CENTER_MEAN = 0x000004,    /* center-shift the waveform                  */
	};

	/// Filter waveforms for visualization purpose, to be used by application.
	/// This is a static utility function, can be used at anytime and any stage of the process.
	/// waveforms	waveforms to be filtered
	/// flags		union of filter options, see WaveformFilterOptions
	virtual pxcStatus PXCAPI FilterWaveformGroup( Data::WaveformGroup* waveforms, WaveformFilterOptions flags)=0;

};

