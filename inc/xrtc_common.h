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

#ifndef _XRTC_COMMON_H_
#define _XRTC_COMMON_H_

#include <string>
#include <vector>
#include <map>

#include "xrtc_api.h"

namespace xrtc {

typedef bool boolean;
typedef std::string DOMString;
#ifdef sequence
#undef sequence
#endif
#define sequence std::vector
    
const DOMString kUnknownKind = "unknown";
const DOMString kAudioKind = "audio";
const DOMString kVideoKind = "video";
    
const DOMString kAudioLabel = "audio_label";
const DOMString kVideoLabel = "video_label";
    
const DOMString kLocalStreamLabel = "local_stream_label";
const DOMString kRemoteStreamLabel = "remote_stream_label";
    
const DOMString kDefaultIceServer = "stun:stun.l.google.com:19302";
    
    
//
// For media type and default consts
enum media_t {
    XRTC_UNKNOWN,
    XRTC_AUDIO,
    XRTC_VIDEO,
};


//
// For capability and constraint

typedef struct constraints_t {
    media_t mtype;
    void *ptr;
    
    constraints_t() : mtype(XRTC_UNKNOWN), ptr(NULL) {}
    constraints_t(constraints_t &other) : mtype(XRTC_UNKNOWN), ptr(NULL) {
        *this = other;
    }
    constraints_t(const constraints_t &other) : mtype(XRTC_UNKNOWN), ptr(NULL) {
        *this = other;
    }
    constraints_t(const audio_constraints_t &other) : mtype(XRTC_AUDIO), ptr(NULL) {
        *this = other;
    }
    constraints_t(const video_constraints_t &other) : mtype(XRTC_VIDEO), ptr(NULL) {
         *this = other;
    }
    virtual ~constraints_t() { release(); }
    
    constraints_t & operator = (const constraints_t &other) {
        if (other.ptr) {
            switch(other.mtype) {
                case XRTC_UNKNOWN: break;
                case XRTC_AUDIO: copyaudio(*(audio_constraints_t *)other.ptr); break;
                case XRTC_VIDEO: copyvideo(*(video_constraints_t *)other.ptr); break;
            }
        }
        return *this;
    }
    constraints_t & operator = (const audio_constraints_t &other) {
        copyaudio(other);
        return *this;
    }
    constraints_t & operator = (const video_constraints_t &other) {
        copyvideo(other);
        return *this;
    }
    
private:
    void copyaudio(const audio_constraints_t &other) {
        audio_constraints_t *audio = new audio_constraints_t;
        audio->aec = other.aec;
        audio->aec2 = other.aec2;
        audio->agc = other.agc;
        audio->agc2 = other.agc2;
        audio->ns = other.ns;
        audio->ns2 = other.ns2;
        audio->highPassFilter = other.highPassFilter;
        audio->typingNosieDetection = other.typingNosieDetection;
        release();
        ptr = audio;
        mtype = XRTC_AUDIO;
    }
    void copyvideo(const video_constraints_t &other) {
        video_constraints_t *video = new video_constraints_t;
        video->device = other.device;
        video->aspectRatio = other.aspectRatio;
        video->width = other.width;
        video->height = other.height;
        video->frameRate = other.frameRate;
        video->noiseReduction = other.noiseReduction;
        video->leakyBucket = other.leakyBucket;
        video->temporalLayeredScreencast = other.temporalLayeredScreencast;
        release();
        ptr = video;
        mtype = XRTC_VIDEO;
    }
    void release() {
        if (ptr) {
            switch(mtype) {
                case XRTC_UNKNOWN: break;
                case XRTC_AUDIO: delete ((audio_constraints_t *)ptr);; break;
                case XRTC_VIDEO: delete ((video_constraints_t *)ptr); break;
            }
            ptr = NULL;
        }
    }
}constraints_t;
    
typedef constraints_t MediaTrackConstraints;
typedef constraints_t MediaConstraints;
typedef media_constraints_t MediaStreamConstraints;


//
// For error message
struct DOMError {
    int errno;
    DOMString errstr;
};

struct NavigatorUserMediaError : public DOMError {
    DOMString constraintName;
};

struct RTCSdpError : public DOMError {
    long sdpLineNumber;
};


//
// For track information
enum MediaStreamTrackState {
    TRACK_NEW,            //"new",
    TRACK_LIVE,           //"live",
    TRACK_ENDED,          //"ended"
};


//
// For Media SDP and ICE 
const DOMString kRTCSdpType[] = {
    "offer",
    "pranswer",
    "answer",
};

struct RTCIceServer {
    sequence<DOMString>     urls;
    DOMString               username;
    DOMString               credential;
};

struct RTCConfiguration {
    sequence<RTCIceServer> iceServers;
};


//
// For peer connection
enum RTCSignalingState {
    SIGNALING_STABLE,       //"stable",
    HAVE_LOCAL_OFFER,       //"have-local-offer",
    HAVE_REMOTE_OFFER,      //"have-remote-offer",
    HAVE_LOCAL_PRANSWER,    //"have-local-pranswer",
    HAVE_REMOTE_PRANSWER,   //"have-remote-pranswer",
    SIGNALING_CLOSED,       //"closed"
};

enum RTCIceGatheringState {
    ICE_NEW,            //"new",
    ICE_GATHERING,      //"gathering",
    ICE_COMPLETE,       //"complete"
};

enum RTCIceConnectionState {
    CONN_NEW,       //"new",
    CHECKING,       //"checking",
    CONNECTED,      //"connected",
    COMPLETED,      //"completed",
    FAILED,         //"failed",
    DISCONNECTED,   //"disconnected",
    CONN_CLOSED,    //"closed"
};


} // namespace xrtc


#endif // _XRTC_COMMON_H_

