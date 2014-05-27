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

class CMediaStream : public MediaStream {
private:
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> m_stream;

public:
bool Init(
        const std::string label, 
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream) 
{
    m_stream = stream;
    if (!m_stream) {
        if (pc_factory) {
            m_stream = pc_factory->CreateLocalMediaStream(label);
        }
    }
    return (m_stream != NULL);
}

explicit CMediaStream ()
{
    m_stream = NULL;
}

virtual ~CMediaStream()
{
    m_stream = NULL;
}

void * getptr() 
{
    return m_stream.get();
}

DOMString id() {
    returnv_assert(m_stream.get(), "")
    return m_stream->label();
}

boolean ended() {
    returnv_assert(m_stream.get(), true)
    return false;
}

sequence<MediaStreamTrackPtr> getAudioTracks ()
{
    sequence<MediaStreamTrackPtr> tracks;
    returnv_assert(m_stream.get(), tracks);

    webrtc::AudioTrackVector atracks = m_stream->GetAudioTracks();
    webrtc::AudioTrackVector::iterator iter = atracks.begin();
    for (; iter != atracks.end(); iter++) {
        MediaStreamTrackPtr track = CreateMediaStreamTrack(XRTC_AUDIO, "", NULL, NULL, (*iter));
        tracks.push_back(track);
    }
    return tracks;
}

sequence<MediaStreamTrackPtr> getVideoTracks ()
{
    sequence<MediaStreamTrackPtr> tracks;
    returnv_assert(m_stream.get(), tracks);

    webrtc::VideoTrackVector vtracks = m_stream->GetVideoTracks();
    webrtc::VideoTrackVector::iterator iter = vtracks.begin();
    for (; iter != vtracks.end(); iter++) {
        MediaStreamTrackPtr track = CreateMediaStreamTrack(XRTC_VIDEO, "", NULL, NULL, (*iter));
        tracks.push_back(track);
    }
    return tracks;
}

MediaStreamTrackPtr getTrackById (DOMString trackId)
{
    MediaStreamTrackPtr track;
    talk_base::scoped_refptr<webrtc::AudioTrackInterface> atrack = m_stream->FindAudioTrack(trackId);
    if (atrack != NULL) {
        track = CreateMediaStreamTrack(XRTC_AUDIO, "", NULL, NULL, atrack);
        return track;
    }

    talk_base::scoped_refptr<webrtc::VideoTrackInterface> vtrack = m_stream->FindVideoTrack(trackId);
    if (vtrack != NULL) {
        track = CreateMediaStreamTrack(XRTC_VIDEO, "", NULL, NULL, vtrack);
        return track;
    }

    return NULL;
}

void addTrack (MediaStreamTrackPtr track)
{
    if (m_stream != NULL && track != NULL) {
        bool bret =  false;
        if (track->kind() == kAudioKind) {
            bret = m_stream->AddTrack((webrtc::AudioTrackInterface *)track->getptr());
        }else if (track->kind() == kVideoKind) {
            bret = m_stream->AddTrack((webrtc::VideoTrackInterface *)track->getptr());
        }
        LOGI("Add "<<track->kind()<<"track to MediaStream, ret="<<bret);
    }
}

void removeTrack (MediaStreamTrackPtr track)
{
    if (m_stream != NULL && track != NULL) {
        bool bret = false;
        if (track->kind() == kAudioKind) {
            bret = m_stream->RemoveTrack((webrtc::AudioTrackInterface *)track->getptr());
        }else if (track->kind() == kVideoKind) {
            bret = m_stream->RemoveTrack((webrtc::VideoTrackInterface *)track->getptr());
        }
        LOGI("Remove "<<track->kind()<<"track to MediaStream, ret="<<bret);
    }
}

//MediaStream              clone ()
//{}

}; //class CMediaStream


ubase::zeroptr<MediaStream> CreateMediaStream(
        const std::string label,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamInterface> pstream) {
    ubase::zeroptr<CMediaStream> stream = new ubase::RefCounted<CMediaStream>();
    if (!stream->Init(label, pc_factory, pstream)) {
        stream = NULL;
    }
    return stream;
}

} // namespace xrtc

