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

#ifndef _XRTC_API_H_
#define _XRTC_API_H_

#include <string>
#include <vector>

#if defined(OBJC)
#import <Foundation/Foundation.h>
#endif

enum action_t {
    ADD_ACTION,
    REMOVE_ACTION,
};

enum ice_state_t {
    kIceNew,
    kIceChecking,
    kIceConnected,
    kIceCompleted,
    kIceFailed,
    kIceDisconnected,
    kIceClosed,
};

enum color_t {
    UnknownFmt = 0,
    I420Fmt,
    RGB24Fmt,
    ARGB32Fmt,
};

enum rotation_t{
  ROTATION_0 = 0,
  ROTATION_90 = 90,
  ROTATION_180 = 180,
  ROTATION_270 = 270
};

// range for any type
template <class T> struct range_t {
    T min;
    T max;
};

// video frame callback from xrtc
typedef struct _video_frame {
    int width;
    int height;
    int color;          // refer to color_t
    int rotation;       // refer to rotation_t
    unsigned long timestamp;
    int length;         // length of video frame
    int size;           // size of data buffer
    unsigned char *data;
}video_frame_t;

// for ice servers: stun and turn server
typedef struct _ice_server {
    std::string uri;
    std::string username;
    std::string password;
}ice_server_t;
typedef std::vector<ice_server_t> ice_servers_t;

// for one constraint of audio or video
template <class T>
struct constraint_t {
    constraint_t() : valid(false), optional(true) {}
    bool valid;     // default the constraint is invald, and will not been used
    bool optional;  // defallt the constraint is optional, mandatory if false
    T val;
};

// for audio constraints
typedef struct _audio_constraints {
    constraint_t<bool> aec;
    constraint_t<bool> aec2;
    constraint_t<bool> agc;
    constraint_t<bool> agc2;
    constraint_t<bool> ns;
    constraint_t<bool> ns2;
    constraint_t<bool> highPassFilter;
    constraint_t<bool> typingNosieDetection;
}audio_constraints_t;

// for video constraints
typedef struct _video_constraints {
    constraint_t<range_t<std::string> > aspectRatio;
    constraint_t<range_t<int> > width;
    constraint_t<range_t<int> > height;
    constraint_t<range_t<int> > frameRate;
    constraint_t<bool> noiseReduction;
    constraint_t<bool> leakyBucket;
    constraint_t<bool> temporalLayeredScreencast;
}video_constraints_t;

// for audio & video constraints in getusermedia
typedef struct _media_constraints {
    bool has_audio;
    audio_constraints_t audio;
    bool has_video;
    video_constraints_t video;
}media_constraints_t;


#if defined(OBJC) // For OBJC intefaces
//>
// The interface of video render:
//  OnSize called when resolution changes.
//  OnFrame called when having decoded data.
@protocol IRtcRender

@required
- (void) OnSize:(int)width height:(int)value;

@required
- (void) OnFrame:(const video_frame_t *)frame;

@end
typedef NSObject<IRtcRender> IRtcRender;


//>
// For receving notification from IRtcCenter
@protocol IRtcSink

// Return media sdp of local a/v, which should be sent to remote peer
@required
- (void) OnSessionDescription:(const std::string &)sdp;

// Return ice candidate of current peer connection, which should be sent to remote peer
@required
- (void) OnIceCandidate:(const std::string &)candidate;

// Notify the status of remote stream(ADD or REMOVE)
// @param action: refer to action_t
@required
- (void) OnRemoteStream:(int)action;

// This callback will be activated when IRtcCenter::GetUserMedia()
// @param error: 0 if OK, else fail
// @param errstr: error message
@required
- (void) OnGetUserMedia:(int)error errstr:(std::string)str;

// This callback for ICE connection state
// @param state: refer to ice_state_t
@required
- (void) OnIceConnectionState:(int)state;

@required
- (void) OnFailureMesssage:(std::string)str;

@end
typedef NSObject<IRtcSink> IRtcSink;

#else

//>
// The interface of video render:
//  OnSize called when resolution changes.
//  OnFrame called when having decoded data.
class IRtcRender {
public:
    virtual ~IRtcRender() {}

    virtual void OnSize(int width, int height) = 0;
    virtual void OnFrame(const video_frame_t *frame) = 0;
};

//>
// For receving notification from IRtcCenter
class IRtcSink {
public:
    virtual ~IRtcSink() {}

    // Return media sdp of local a/v, which should be sent to remote peer
    virtual void OnSessionDescription(const std::string &sdp) = 0;

    // Return ice candidate of current peer connection, which should be sent to remote peer
    virtual void OnIceCandidate(const std::string &candidate) = 0;

    // Notify the status of remote stream(ADD or REMOVE)
    // @param action: refer to action_t
    virtual void OnRemoteStream(int action) = 0;

    // This callback will be activated when IRtcCenter::GetUserMedia()
    // @param error: 0 if OK, else fail
    // @param errstr: error message
    virtual void OnGetUserMedia(int error, std::string errstr) = 0;

    // This callback for ICE connection state
    // @param state: refer to ice_state_t
    virtual void OnIceConnectionState(int state) = 0;

    virtual void OnFailureMesssage(std::string errstr) = 0;
};

#endif // OBJC


//>
// Webrtc Control Center
class IRtcCenter {
public:
    virtual ~IRtcCenter() {}
    virtual void SetSink(IRtcSink *sink) = 0;

    // To get local stream of audio & video, SUCCESS or fail by IRtcSink::OnGetUserMedia()
    // @param constraints: [in] refer to media_constraints_t
    // @return 0 if OK, else fail
    virtual long GetUserMedia(media_constraints_t constraints) = 0;

    // To create peer conncetion
    // @return 0 if OK, else fail
    virtual long CreatePeerConnection() = 0;
    virtual long CreatePeerConnection(ice_servers_t servers) = 0;

    // To add local stream (got by GetUserMedia) into peer connection(got by CreatePeerConnection)
    // @return 0 if OK, else fail
    virtual long AddLocalStream() = 0;

    // To set render for local video, only valid after receiving IRtcSink::OnGetUserMedia()
    // @param render: [in] UI Render
    // @param action: [in] refer to action_t
    // @return 0 if OK, else fail
    virtual long SetLocalRender(IRtcRender *render, int action) = 0;

    // To set render for remote video, only valid after receiving IRtcSink::OnRemoteStream()
    // @param render: [in] UI Render
    // @param action: [in] refer to action_t
    // @return 0 if OK, else fail
    virtual long SetRemoteRender(IRtcRender *render, int action) = 0;

    // To initiate a/v call to remote peer
    // @return 0 if OK, else fail
    virtual long SetupCall() = 0;

    // To answer a/v call from remote peer
    // @return 0 if OK, else fail
    virtual long AnswerCall() = 0;

    // To close call
    virtual void Close() = 0;

    // To set sdp of local media into current peer connection
    // @param sdp:  [in] sdp of local media (json format, type: offer/pranswer/answer)
    // @return 0 if OK, else fail
    virtual long SetLocalDescription(const std::string &sdp) = 0;

    // To set sdp of remote peer into current peer connection
    // @param sdp:  [in] sdp of remote media (json format, type: offer/pranswer/answer)
    // @return 0 if OK, else fail
    virtual long SetRemoteDescription(const std::string &sdp) = 0;

    // To add remote ice candidate into current peer connection
    // @param candidate:     [in] ice candidate(json format)
    // @return 0 if OK, else fail
    virtual long AddIceCandidate(const std::string &candidate) = 0;
};

extern "C" {
bool        xrtc_init();
void        xrtc_uninit();
bool        xrtc_create(IRtcCenter * &prtc);
void        xrtc_destroy(IRtcCenter * prtc);
}


#endif // _XRTC_API_H_

