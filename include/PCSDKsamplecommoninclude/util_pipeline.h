/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "util_pipeline_face.h"
#include "util_pipeline_emotion.h"
#include "util_pipeline_gesture.h"
#include "util_pipeline_raw.h"
#include "util_pipeline_voice.h"
#include "util_pipeline_segmentation.h"

class UtilPipeline:public UtilPipelineEmotion, public UtilPipelineSegmentation, public UtilPipelineVoice, public UtilPipelineGesture, public UtilPipelineFace, public UtilPipelineRaw {
public:
	UtilPipeline(PXCSession *session=NULL, const pxcCHAR *file=0, bool recording=false):UtilPipelineEmotion(),UtilPipelineSegmentation(),UtilPipelineVoice(),UtilPipelineGesture(),UtilPipelineFace(),UtilPipelineRaw(session,file,recording,6) {
		UtilPipelineSegmentation::m_next=dynamic_cast<UtilPipelineEmotion*>(this);
		UtilPipelineVoice::m_next=dynamic_cast<UtilPipelineSegmentation*>(this);
		UtilPipelineGesture::m_next=dynamic_cast<UtilPipelineVoice*>(this);
		UtilPipelineFace::m_next=dynamic_cast<UtilPipelineGesture*>(this);
		UtilPipelineRaw::m_next=dynamic_cast<UtilPipelineFace*>(this);
	}
    virtual void PXCAPI Release(void) { delete this; }
};

