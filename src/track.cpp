/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 PeterXu uskee521@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "xrtc_std.h"
#include "webrtc.h"
#include "error.h"

namespace xrtc {

static sequence<SourceInfo> _audio_sources;
static sequence<SourceInfo> _video_sources;

class CMediaStreamTrack : public MediaStreamTrack {
private:
    talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> m_track;
    talk_base::scoped_refptr<webrtc::MediaSourceInterface> m_source;

    MediaTrackConstraints m_constraints;
    MediaSourceStates m_source_states;
    AllMediaCapabilities *m_capabilities;

public:
bool Init(
        const std::string kind, 
        const std::string label,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory,
        talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    m_track = track;
    if (m_track != NULL) {
        if (kind == kAudioKind)
            m_source = ((webrtc::AudioTrackInterface *)m_track.get())->GetSource();
        else
            m_source = ((webrtc::VideoTrackInterface *)m_track.get())->GetSource();
        return true;
    }

    returnv_assert(pc_factory.get(), false);
    if (kind == kAudioKind) {
        if (!m_source.get())
            m_source = pc_factory->CreateAudioSource(NULL);
        m_track = pc_factory->CreateAudioTrack(label, (webrtc::AudioSourceInterface *)(m_source.get()));
    }else if (kind == kVideoKind) {
        if (!m_source.get()) {
            std::string vname = "";
            sequence<MediaTrackConstraint>::iterator iter;
            for (iter=m_constraints.optional.begin(); iter != m_constraints.optional.end(); iter++) {
                if (iter->first != "sourceId") 
                    continue;
                sequence<SourceInfo>::iterator iter2;
                for (iter2=_video_sources.begin(); iter2 != _video_sources.end(); iter2++) {
                    if (iter2->sourceId == iter->second) {
                        vname = iter2->label;
                        break;
                    }
                }
                break;
            }

            // if vname empty, select default device
            LOGI("vname="<<vname);
            cricket::VideoCapturer* capturer = OpenVideoCaptureDevice(vname);
            if (capturer) {
                m_source = pc_factory->CreateVideoSource(capturer, NULL);
            }
        }

        LOGD("create video track by source");
        if (m_source) {
            m_track = pc_factory->CreateVideoTrack(label, (webrtc::VideoSourceInterface *)(m_source.get()));
        }
    }
    return (m_track != NULL);
}

explicit CMediaStreamTrack(MediaTrackConstraints *constraints)
{
    if (constraints) {
        m_constraints.mandatory = constraints->mandatory;
        m_constraints.optional = constraints->optional;
    }
}

virtual ~CMediaStreamTrack()
{
    m_source.release();
    m_track.release();
}

void * getptr()
{
    return m_track.get();
}

//
// For attribute of MediaSteamTrack
DOMString id() {
    returnv_assert(m_track.get(), "");
    return m_track->id();
}
DOMString kind() {
    returnv_assert(m_track.get(), "");
    return m_track->kind();
}
DOMString label() {
    returnv_assert(m_track.get(), "");
    return m_track->id();
}
boolean enabled() {
    returnv_assert(m_track.get(), false);
    return m_track->enabled();
}
void Put_enabled(boolean enable) {
    return_assert(m_track.get());
    m_track->set_enabled(enable);
}
boolean muted() {
    returnv_assert(m_source.get(), false);
    webrtc::MediaSourceInterface::SourceState state = m_source->state();
    return (state == webrtc::MediaSourceInterface::kMuted);
}
MediaStreamTrackState readyState() {
    returnv_assert(m_track.get(), TRACK_ENDED);

    MediaStreamTrackState state = TRACK_ENDED;
    webrtc::MediaStreamTrackInterface::TrackState tstate = m_track->state();
    switch (tstate) {
    case webrtc::MediaStreamTrackInterface::kInitializing: 
        state = TRACK_NEW; 
        break;
    case webrtc::MediaStreamTrackInterface::kLive: 
        state = TRACK_LIVE; 
        break;
    case webrtc::MediaStreamTrackInterface::kEnded: 
    case webrtc::MediaStreamTrackInterface::kFailed: 
        state = TRACK_ENDED; 
        break;
    }
    return state;
}

MediaTrackConstraints constraints()
{
    return m_constraints;
}

MediaSourceStates states()
{
    return m_source_states;
}

AllMediaCapabilities * capabilities()
{
    return m_capabilities;
}

void applyConstraints(MediaTrackConstraints &constraints)
{
    m_constraints = constraints;
}

//MediaStreamTrack clone()
//{}

void stop()
{}


///
/// for device
static cricket::VideoCapturer* OpenVideoCaptureDevice(std::string vid)
{
    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        LOGW("fail to init DeviceManager");
        return NULL;
    }

    LOGD("device id="<<vid);
    cricket::VideoCapturer* capturer = NULL;

#if 0
    std::vector<cricket::Device> devices;
    if(!dev_manager->GetVideoCaptureDevices(&devices)) {
        LOGW("fail to GetVideoCaptureDevices");
        return NULL;
    }

    std::vector<cricket::Device>::iterator iter = devices.begin();
    for (; iter != devices.end(); iter++) {
        std::string key = (*iter).id;
        if (!vid.empty() && vid != key) {
            continue;
        }
        capturer = dev_manager->CreateVideoCapturer(*iter);
        if (capturer != NULL)
            break;
    }
#else
    cricket::Device device;
    if(!dev_manager->GetVideoCaptureDevice(vid, &device)) {
        LOGW("fail to GetVideoCaptureDevice");
        return NULL;
    }
    capturer = dev_manager->CreateVideoCapturer(device);
#endif

    // TODO: choose the best format
    if (capturer) {
        const std::vector<cricket::VideoFormat>* formats = capturer->GetSupportedFormats();
        LOGD("supported format size="<<formats->size());
        for (int k=0; k < formats->size(); k++) {
            const cricket::VideoFormat & format = (*formats)[k];
            LOGD("supported format, width="<<format.width<<", height="<<format.height);
        }
    }
    return capturer;
}

}; //class CMediaStreamTrack


static bool GetSourceInfos(const std::string kind, sequence<SourceInfo> &sources) {
    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        return false;
    }

    std::vector<cricket::Device> devs;
    if (kind == kVideoKind) {
        dev_manager->GetVideoCaptureDevices(&devs);
    }else if (kind == kAudioKind) {
        // dev_manager->GetAudioOutputDevices(&devs);
        dev_manager->GetAudioInputDevices(&devs);
    }else {
        return false;
    }

    SourceInfo source;
    source.kind = kind;
    std::vector<cricket::Device>::iterator iter;
    for (iter=devs.begin(); iter != devs.end(); ++iter) {
        source.sourceId = (*iter).id;
        source.label = (*iter).name;
        sources.push_back(source);
    }
    devs.clear();

    return true;
}

sequence<SourceInfo> & VideoStreamTrack::getSourceInfos() {
    _video_sources.clear();
    GetSourceInfos(kVideoKind, _video_sources);
    return _video_sources;
}

sequence<SourceInfo> & AudioStreamTrack::getSourceInfos() {
    _audio_sources.clear();
    GetSourceInfos(kAudioKind, _audio_sources);
    return _audio_sources;
}


ubase::zeroptr<MediaStreamTrack> CreateMediaStreamTrack(
        media_t mtype,
        const std::string label,
        MediaTrackConstraints *constraints,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> ptrack)
{
    std::string kind;
    switch(mtype) {
    case XRTC_AUDIO: kind = kAudioKind; break;
    case XRTC_VIDEO: kind = kVideoKind; break;
    default: return NULL;
    }

    ubase::zeroptr<CMediaStreamTrack> track = new ubase::RefCounted<CMediaStreamTrack>(constraints);
    if (!track->Init(kind, label, pc_factory, ptrack)) {
        track = NULL;
    }
    return track;
}


} // namespace xrtc

