/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcface.h
    Defines the PXCFaceAnalysis interfaces, which supports face analysis.
 */
#pragma once
#include "pxcscheduler.h"
#include "pxcaccelerator.h"
#include "pxccapture.h" 

/**
    This interface defines a standard abstraction for face analysis
    algorithms such as face location detection, landmark detection,
    recognition, and attribute detection. 
 */
class PXCFaceAnalysis:public PXCBase { // Extend Detection, Landmark etc.
public:
    PXC_CUID_OVERWRITE(PXC_UID('F','A','N','S'));

    struct ProfileInfo {
        PXCCapture::VideoStream::DataDesc    inputs;            /* required input stream formats */
        pxcBool                              iftracking;        /* =true for video sequence tracking */
        pxcU32                               reserved[7];
    };

    class Detection:public PXCBase {
    public:
        PXC_CUID_OVERWRITE(PXC_UID('D','E','C','T'));

        /* Face Detection Algorithm View Angle: bit-OR'ed values */
        typedef pxcEnum ViewAngle;
        enum {
            // multi-profile detection
            VIEW_ANGLE_0          = 0x00000001,
            VIEW_ANGLE_45         = 0x00000002,
            VIEW_ANGLE_FRONTAL    = 0x00000004,
            VIEW_ANGLE_135        = 0x00000008,
            VIEW_ANGLE_180        = 0x00000010,

            // multi-roll detection
            VIEW_ROLL_30          = 0x00000020,
            VIEW_ROLL_30N         = 0x00000040,
            VIEW_ROLL_60          = 0x00000080,
            VIEW_ROLL_60N         = 0x00000100,

            VIEW_ANGLE_HALF_MULTI = VIEW_ANGLE_FRONTAL | VIEW_ANGLE_45 | VIEW_ANGLE_135,
            VIEW_ANGLE_MULTI      = VIEW_ANGLE_HALF_MULTI | VIEW_ANGLE_0 | VIEW_ANGLE_180, //=0x0000001F
            VIEW_ANGLE_FRONTALROLL= VIEW_ANGLE_FRONTAL | VIEW_ROLL_30| VIEW_ROLL_30N | VIEW_ROLL_60 | VIEW_ROLL_60N, //=0x000001E4
            VIEW_ANGLE_OMNI       = 0xFFFFFFFF,
        };

        struct ProfileInfo {
            PXCSizeU32  minFaceSize;    /* minimum face size */
            PXCSizeU32  maxFaceSize;    /* maximum face size */
            pxcU64      reserved;
            ViewAngle   viewAngles;     /* algorithm limitation on face view angles */
            pxcU32      responsiveness; 
        };

        struct Data {
            PXCRectU32  rectangle;      /* face rectangle */
            pxcUID      fid;            /* face identifier */
            pxcU32      confidence;     /* 0-100 */
            ViewAngle   viewAngle;      /* face angle */
            pxcU32      reserved[4];    /* Yaw, Roll, Pitch: UNKNOWN */
        };

        virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;
        pxcStatus PXCAPI QueryProfile(ProfileInfo *pinfo) { return QueryProfile((pxcU32)WORKING_PROFILE,pinfo); }
        virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;
        virtual pxcStatus PXCAPI QueryData(pxcUID fid, Data *data)=0;
    };

    class Landmark:public PXCBase {
    public:
        PXC_CUID_OVERWRITE(PXC_UID('L','D','M','R'));

        typedef pxcEnum Label;
        enum {
            LABEL_LEFT_EYE_OUTER_CORNER   =0x0001000,
            LABEL_LEFT_EYE_INNER_CORNER   =0x0002000,
            LABEL_RIGHT_EYE_OUTER_CORNER  =0x0004000,
            LABEL_RIGHT_EYE_INNER_CORNER  =0x0008000,
            LABEL_MOUTH_LEFT_CORNER       =0x0010000,
            LABEL_MOUTH_RIGHT_CORNER      =0x0020000,
            LABEL_NOSE_TIP                =0x0040000,

            LABEL_6POINTS                 =0x003F006,
            LABEL_7POINTS                 =0x007F007,
            LABEL_SIZE_MASK               =0x0000FFF,
        };

        struct ProfileInfo {
            Label   labels;     /* enumeration of supported feature points */
            pxcU32  reserved[7];
        };

        struct PoseData {
            pxcUID          fid;
            pxcF32          yaw, roll, pitch;
            pxcU32          reserved[4];
        };

        struct LandmarkData {
            PXCPoint3DF32   position;
            pxcUID          fid;
            Label           label;
            pxcU32          lidx;
            pxcU32          reserved[6];
        };

        virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;
        pxcStatus PXCAPI QueryProfile(ProfileInfo *pinfo) { return QueryProfile((pxcU32)WORKING_PROFILE,pinfo); }
        virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;
        virtual pxcStatus PXCAPI QueryLandmarkData(pxcUID fid, Label landmark, pxcU32 lidx, LandmarkData *data)=0;
        pxcStatus __inline QueryLandmarkData(pxcUID fid, Label landmark, LandmarkData *data) {
            pxcStatus sts=PXC_STATUS_ITEM_UNAVAILABLE;
            for (pxcU32 i=0;i<(pxcU32)((landmark&LABEL_SIZE_MASK)?(landmark&LABEL_SIZE_MASK):1);i++)
                if (QueryLandmarkData(fid,landmark,i,data+i)>=PXC_STATUS_NO_ERROR) sts=PXC_STATUS_NO_ERROR; else data[i].label=0;
            return sts;
        }

        virtual pxcStatus PXCAPI QueryPoseData(pxcUID /*fid*/, PoseData * /*pose*/)=0;
    };

    class Recognition:public PXCBase {        /* extend Recognition::Database */
    public:
        PXC_CUID_OVERWRITE(PXC_UID('F','R','E','C'));

        struct ProfileInfo {
            pxcU32    modelSize;        // bytes in saving a serialized model.
            pxcF32    threshold;        // threshold for the comparison score.
            pxcU32  reserved[6];    
        };

        class Model:public PXCBase {
        public:
            PXC_CUID_OVERWRITE(PXC_UID('F','R','C','M'));

            virtual pxcStatus PXCAPI Serialize(pxcBYTE *buffer)=0;
            virtual pxcStatus PXCAPI Compare(Model **models, pxcU32 nmodels, pxcF32 *scores, pxcU32 *index)=0;
            pxcStatus __inline Compare(Model **models, pxcU32 nmodels, pxcU32 *index) { return Compare(models,nmodels,0,index); }
        };

        virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;
        pxcStatus PXCAPI QueryProfile(ProfileInfo *pinfo) { return QueryProfile((pxcU32)WORKING_PROFILE,pinfo); }
        virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;
        virtual pxcStatus PXCAPI CreateModel(pxcUID fid, Model **model)=0;
        virtual pxcStatus PXCAPI DeserializeModel(pxcBYTE *buffer, Model **model)=0;
    };

    virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *profile)=0;
    pxcStatus __inline QueryProfile(ProfileInfo *profile) { return QueryProfile((pxcU32)WORKING_PROFILE,profile); }
    virtual pxcStatus PXCAPI SetProfile(ProfileInfo *profile)=0;

    virtual pxcStatus PXCAPI QueryFace(pxcU32 fidx, pxcUID *fid, pxcU64 *timeStamp)=0;
    pxcStatus __inline QueryFace(pxcU32 fidx, pxcUID *fid) { return QueryFace(fidx,fid,0); }
    virtual pxcStatus PXCAPI ProcessImageAsync(PXCImage *images[], PXCScheduler::SyncPoint **sp)=0;
};