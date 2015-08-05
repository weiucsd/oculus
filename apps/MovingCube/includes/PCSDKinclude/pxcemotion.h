/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file PXCEmotion.h
    Defines the PXCEmotion interface, which programs may use to detect emotions
    based on real-time facial expression analysis.
 */
#pragma once
#include "pxcsession.h"
#include "pxccapture.h"
#pragma warning(push)
#pragma warning(disable:4201) /* nameless structs/unions */

/**
    This class defines a standard interface for emotion detection algorithms.
 */
class PXCEmotion:public PXCBase {
public:
    PXC_CUID_OVERWRITE(PXC_UID('E','M','T','N'));

    /// The Emotion Data Structure  
    struct EmotionData {
        typedef pxcEnum Emotion;
        enum {
            EMOTION_PRIMARY_ANGER		=0x00000001,	// primary emotion ANGER
            EMOTION_PRIMARY_CONTEMPT	=0x00000002,	// primary emotion CONTEMPT
			EMOTION_PRIMARY_DISGUST		=0x00000004,	// primary emotion DISGUST
			EMOTION_PRIMARY_FEAR		=0x00000008,	// primary emotion FEAR
			EMOTION_PRIMARY_JOY			=0x00000010,	// primary emotion JOY
			EMOTION_PRIMARY_SADNESS		=0x00000020,	// primary emotion SADNESS
			EMOTION_PRIMARY_SURPRISE	=0x00000040,	// primary emotion SURPRISE

            EMOTION_SENTIMENT_POSITIVE  =0x00010000,	// Overall sentiment: POSITIVE
            EMOTION_SENTIMENT_NEGATIVE  =0x00020000,	// Overall sentiment: NEGATIVE
			EMOTION_SENTIMENT_NEUTRAL   =0x00040000,	// Overall sentiment: NEUTRAL
        };

        pxcU64          timeStamp;                // Time stamp in 100ns when the emotion data is detected
        pxcU32          fid;                      // Face ID
		Emotion			eid;					  // Emotion identifier
		pxcF32			intensity;				  // In range [0,1]. The intensity value is the detection output
												  // to indicate the presence likelihood of an emotion:
												  //     [0.0, 0.2): expression is likely absent
												  //     [0.2, 0.4): expression is of low intensity
												  //     [0.4, 0.6): expression is of medium intensity
												  //     [0.6, 0.8): expression is of high intensity
												  //     [0.8, 1.0]: expression is of very high intensity
		pxcI32			evidence;                 // The evidence value, between -5 and  5, represents the odds in 
		                                          // 10-based logaritmic scale of a target expression being present.
		                                          // For instance,
		                                          //   Value 2 indicates that an emotion is 100 (10^2) times more likely
		                                          //   to be categorized as its presence than not presence; while
		                                          //   Value -2 indicates that an emotion is 100 times more likely
										          //   to be categorized as its not presence than presence. 
        PXCRectU32		rectangle;				  // Detected face rectangle
        pxcU32          reserved[8];
    };


    /// The Emotion Module Configuration Structure
    struct ProfileInfo {
        PXCCapture::VideoStream::DataDesc inputs;   // Module input requirements
        EmotionData::Emotion              sets;     // Bit-OR'ed value to specify sets of emotions that should be detected
        pxcU32							  reserved[6];          
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

    /// Query the total number of detected faces for a given frame.
	///   Note: face id (fid) will be zero-based index in  
    virtual pxcU32 PXCAPI QueryNumFaces()=0;

    /// Get Emotion data of the specified face and emotion.
    /// fid     The face ID, zero-based
    /// eid     The emotion identifier
    /// data    The EmotionData data structure, to be returned
    virtual pxcStatus PXCAPI QueryEmotionData(pxcU32 /*fid*/, EmotionData::Emotion /*eid*/, EmotionData * /*data*/) =0;

	/// Get all Emotion data of a specified face.
    /// fid      The face ID, zero-based
    /// data     The array of EmotionData data structures, to be returned
	///			 Application should allocate the EmotionData array of size 10 for all 10 emotions  	
    virtual pxcStatus QueryAllEmotionData(pxcU32 /*fid*/, EmotionData * /*data*/) =0;  


    /// Process the input image(s) for emotion detection.
    /// images      The input image arrays
    /// sp          The sync point, to be returned
    virtual pxcStatus PXCAPI ProcessImageAsync(PXCImage *images[], PXCScheduler::SyncPoint **sp)=0;

};
#pragma warning(pop)