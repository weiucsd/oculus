/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include <vector>
#include <list>
#include <map>
#include "service/pxcsmartasyncimpl.h"
#include "pxccapture.h"
#include "pxcsession.h"
#include "pxcsmartptr.h"

class UtilCapture {
public:

    UtilCapture(PXCSession *session);
    virtual ~UtilCapture(void);
	virtual void Release(void) { delete this; }

    virtual void SetFilter(PXCSession::ImplDesc &desc)                  { m_desc_filter=desc; }
	virtual void SetFilter(PXCImage::ImageType type, PXCSizeU32 &size, pxcU32 fps=0);
	virtual void SetFilter(PXCCapture::VideoStream::ProfileInfo *pinfo)	{ m_vstream_filters.push_back(*pinfo); }
	virtual void SetFilter(pxcCHAR *name)								{ wcscpy_s<sizeof(m_device_filter.name)/sizeof(pxcCHAR)>(m_device_filter.name,name); }
	virtual void SetFilter(PXCCapture::DeviceInfo *dinfo)               { m_device_filter=(*dinfo); }
	virtual void SetFilter(PXCCapture::Device::Property p, pxcF32 v)	{ m_devcap_filters[p]=v; }

    virtual pxcStatus LocateStreams(PXCCapture::AudioStream::DataDesc *inputs);
    virtual pxcStatus LocateStreams(PXCCapture::VideoStream::DataDesc *inputs);
    virtual pxcStatus LocateStreams(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
    virtual pxcStatus LocateStreams(std::vector<PXCCapture::AudioStream::DataDesc*> &ainputs);
    virtual pxcStatus LocateStreams(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, std::vector<PXCCapture::AudioStream::DataDesc*> &ainputs);

    virtual PXCCapture::Device      *QueryDevice(void)              { return m_device; }
    virtual PXCCapture::VideoStream *QueryVideoStream(int channel, int view=0);
    virtual PXCCapture::AudioStream *QueryAudioStream(void)         { return m_astream; }
    virtual PXCCapture              *QueryCapture(void)             { return m_capture; }
    virtual PXCImage                *QueryImage(PXCImage *images[], PXCImage::ImageType type);
    virtual PXCImage                *QueryImageByFormat(PXCImage *images[], PXCImage::ColorFormat format);
    virtual pxcStatus                MapImages(int view, PXCImage *images_in[], PXCImage *images_out[]);

    virtual pxcStatus ReadStreamAsync(PXCImage *images[], PXCScheduler::SyncPoint **sp);
    virtual pxcStatus ReadStreamAsync(PXCAudio **audio, PXCScheduler::SyncPoint **sp);

protected:

    friend class PXCSmartAsyncImplIxOy<UtilCapture,PXCImage,PXCCapture::VideoStream::STREAM_LIMIT,PXCScheduler::SyncPoint,1>;

    PXCSession*                 m_session;
    PXCSessionService*          m_session_service;
    PXCSmartPtr<PXCScheduler>   m_scheduler;
    std::list<PXCCapture::VideoStream::ProfileInfo>			m_vstream_filters;
    std::map<PXCCapture::Device::Property,pxcF32>           m_devcap_filters;
    PXCSession::ImplDesc        m_desc_filter;
    PXCCapture::DeviceInfo      m_device_filter;

    PXCSmartPtr<PXCCapture>                                 m_capture;
    PXCSmartPtr<PXCCapture::Device>                         m_device;
    std::vector<PXCCapture::VideoStream*>                   m_vstreams;
    PXCSmartPtr<PXCCapture::AudioStream>                    m_astream;
    std::vector< std::vector<int> >                         m_maps;
   
    virtual pxcStatus PXCAPI CreateCapture(pxcU32 index, PXCCapture **capture);
    virtual pxcStatus PXCAPI ReadStreamSync(PXCImage *images[], PXCScheduler::SyncPoint *spts[]);
	virtual pxcStatus PXCAPI PassOnStatus(pxcStatus sts) { return sts; }

    virtual int  MatchProfiles(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, PXCCapture::Device::StreamInfo &sinfo, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream);
    virtual void ScanProfiles(std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, PXCCapture::VideoStream *vstream);
    virtual bool RecordProperties(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
    virtual void FindBestProfile(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, std::list<PXCCapture::VideoStream::ProfileInfo> &profiles, int vstream);
	virtual bool ConsolidateAudioRequests(std::vector<PXCCapture::AudioStream::DataDesc*> &ainputs,PXCCapture::AudioStream::DataDesc *ainput);
    virtual void DeleteStreams(void);
    virtual int  CalculateNumFormats(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
    virtual void ClearMaps(std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs);
};

