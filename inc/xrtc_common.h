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

namespace xrtc {

typedef bool boolean;
typedef std::string DOMString;
#ifdef sequence
#undef sequence
#endif
#define sequence std::vector


//
// For capability and constraint
typedef std::map<DOMString, DOMString> MediaTrackConstraintSet;
typedef std::pair<DOMString, DOMString> MediaTrackConstraint;
typedef std::map<DOMString, DOMString> MediaConstraints;


struct MediaTrackConstraints {
    MediaTrackConstraintSet mandatory;
    sequence<MediaTrackConstraint> optional;
};

struct MediaStreamConstraints {
    boolean video;
    MediaTrackConstraints videoConstraints;
    boolean audio;
    MediaTrackConstraints audioConstraints;
};


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
// For media type and default consts
enum media_t {
    XRTC_UNKNOWN,
    XRTC_AUDIO,
    XRTC_VIDEO,
};

const DOMString kUnknownKind = "unknown";
const DOMString kAudioKind = "audio";
const DOMString kVideoKind = "video";

const DOMString kAudioLabel = "audio_label";
const DOMString kVideoLabel = "video_label";

const DOMString kLocalStreamLabel = "local_stream_label";
const DOMString kRemoteStreamLabel = "remote_stream_label";

const DOMString kDefaultIceServer = "stun:stun.l.google.com:19302";



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

