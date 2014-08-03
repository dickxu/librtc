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


// action of remote stream
enum action_t {
    kAddStream,
    kRemoveStream,
};


//>
// for ice servers: stun and turn server
typedef struct _ice_server {
    std::string uri;
    std::string username;
    std::string password;
}ice_server_t;
typedef std::vector<ice_server_t> ice_servers_t;

// state of ice connection
enum ice_conn_state_t {
    kIceConnNew,
    kIceConnChecking,
    kIceConnConnected,
    kIceConnCompleted,
    kIceConnFailed,
    kIceConnDisconnected,
    kIceConnClosed,
};


//>
// colorspace of video frame 
enum color_t {
    kUnknownFmt = 0,
    kI420Fmt,
    kRGB24Fmt,
    kARGB32Fmt,
};

// rotation degree of video frame
enum rotation_t{
    kRotation_0     = 0,
    kRotation_90    = 90,
    kRotation_180   = 180,
    kRotation_270   = 270
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


//>
// for device's type
enum device_kind_t {
    kDeviceNone,            //"none",
    kVideoCapture,          //"camera",
    kAudioIn,               //"microphone"
    kAudioOut,              //"speaker"
};

// for device info
typedef struct _device {
    int kind;               //refer to device_kind_t
    std::string name;       //readable name of device
    std::string did;        //device id
    
    _device() : kind(kDeviceNone) {}
    _device(const _device &other) { *this = other; }
    _device(int kind, const std::string name, const std::string did) : kind(kind), name(name), did(did) {}
    _device & operator = (const _device &other) {
        if (this == &other) return *this;
        this->kind = other.kind;
        this->name = other.name;
        this->did = other.did;
        return *this;
    }
}device_t;
typedef std::vector<device_t> devices_t;


//>
// for one constraint of audio or video
template <class T>
struct constraint_t {
    T val;
    bool valid;     // default the constraint is invald, and will not been used
    bool optional;  // defallt the constraint is optional, mandatory if false
    
    constraint_t() : valid(false), optional(true) {}
    constraint_t(const constraint_t &other) { *this = other; }
    constraint_t(const T &val, bool mandatory=true) : val(val), valid(true), optional(!mandatory) {}
    constraint_t & operator = (const constraint_t &other) {
        if (this == &other) return *this;
        this->val = other.val;
        this->valid = other.valid;
        this->optional = other.optional;
        return *this;
    }
};

// range for any type
template <class T> struct range_t {
    T min;
    T max;
    
    range_t() {}
    range_t(const range_t &other) { *this = other; }
    range_t(const T &min, const T &max) : min(min), max(max) {}
    range_t & operator = (const range_t &other) {
        if (this == &other) return *this;
        this->min = other.min;
        this->max = other.max;
        return *this;
    }
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
    
    explicit _audio_constraints() {}
}audio_constraints_t;

// for video constraints
typedef struct _video_constraints {
    constraint_t<device_t> device;
    constraint_t<range_t<std::string> > aspectRatio;
    constraint_t<range_t<int> > width;
    constraint_t<range_t<int> > height;
    constraint_t<range_t<int> > frameRate;
    constraint_t<bool> noiseReduction;
    constraint_t<bool> leakyBucket;
    constraint_t<bool> temporalLayeredScreencast;
    
    explicit _video_constraints() {}
}video_constraints_t;


// for audio & video constraints in getusermedia
typedef struct _media_constraints {
    bool has_audio;
    audio_constraints_t audio;
    bool has_video;
    video_constraints_t video;
    
    explicit _media_constraints() {}
}media_constraints_t;


//>
// The interface of video render
#if defined(OBJC) // For OBJC intefaces
@protocol IRtcRender

@required
- (void) OnSize:(int)width height:(int)value;

@required
- (void) OnFrame:(const video_frame_t *)frame;

@end
typedef NSObject<IRtcRender> IRtcRender;

#else

class IRtcRender {
public:
    virtual ~IRtcRender() {}

    // Called when resolution of decoded frame changes.
    // @param width: width of decoded frame
    // @param height: height of decoded frame
    virtual void OnSize(int width, int height) = 0;

    // Called when having decoded frame.
    // @param frame: refer to video_frame_t, return decoded frame
    virtual void OnFrame(const video_frame_t *frame) = 0;
};

#endif // OBJC


//>
// For receving notification from IRtcCenter
#if defined(OBJC) // For OBJC intefaces
@protocol IRtcSink

@required
- (void) OnSessionDescription:(const std::string &)sdp;

@required
- (void) OnIceCandidate:(const std::string &)candidate;

@required
- (void) OnRemoteStream:(int)action;

@required
- (void) OnGetUserMedia:(int)error errstr:(std::string)str;

@required
- (void) OnIceConnectionState:(int)state;

@required
- (void) OnFailure:(std::string) message;

@required
- (void) OnError;

@end
typedef NSObject<IRtcSink> IRtcSink;

#else

class IRtcSink {
public:
    virtual ~IRtcSink() {}

    // Return media sdp of local a/v, which should be sent to remote peer
    // @param sdp: [out] sdp of local a/v (json format)
    virtual void OnSessionDescription(const std::string &sdp) = 0;

    // Return ice candidate of current peer connection, which should be sent to remote peer
    // @param candidate: [out] ice candidate (json format)
    virtual void OnIceCandidate(const std::string &candidate) = 0;

    // Notify the status of remote stream(ADD or REMOVE)
    // @param action: [out] status of remote stream, refer to action_t
    virtual void OnRemoteStream(int action) = 0;

    // This callback will be activated when IRtcCenter::GetUserMedia()
    // @param error: [out] 0 if OK, else fail
    // @param errstr: [out] error message
    virtual void OnGetUserMedia(int error, std::string errstr) = 0;

    // This callback for ICE connection state
    // @param state: [out] state of ice connection, refer to ice_conn_state_t
    virtual void OnIceConnectionState(int state) = 0;

    // This callback will be activated when error happens in webrtc(session)
    // @param message: [out] failure message of internel errors
    virtual void OnFailure(std::string message) = 0;

    // This callback will be activated when error happens in peer connection
    virtual void OnError() = 0;
};

#endif // OBJC


//>
// Webrtc Control Center
class IRtcCenter {
public:
    virtual ~IRtcCenter() {}

    // To set sink of IRtcCenter for receiving the status of RTC
    // @param sink: [in] object of sink
    virtual void SetSink(IRtcSink *sink) = 0;

    // To get system audio/video devices
    // @param devices: [in] device kind(audio/video), refer to device_kind_t
    // @param devices: [out] system devices, refer to devices_t
    virtual void GetDevices(const device_kind_t kind, devices_t & devices) = 0;

    // To get local stream of audio & video, SUCCESS or fail indicated by IRtcSink::OnGetUserMedia()
    // @param constraints: [in] media constrainsts(audio/video), refer to media_constraints_t
    // @return 0 if OK, else fail
    virtual long GetUserMedia(const media_constraints_t & constraints) = 0;

    // To create peer conncetion, default with google stun server
    // @return 0 if OK, else fail
    virtual long CreatePeerConnection() = 0;

    // To create peer conncetion
    // @param servers: [in] stun/turn server list, refer to ice_servers_t
    // @return 0 if OK, else fail
    virtual long CreatePeerConnection(const ice_servers_t & servers) = 0;

    // To add local stream into peer connection,
    //      which should be callbed after success of both GetUserMedia and CreatePeerConnection.
    // @return 0 if OK, else fail
    virtual long AddLocalStream() = 0;

    // To set render for local video, only valid after receiving IRtcSink::OnGetUserMedia()
    // @param render: [in] object of UI Render
    // @param action: [in] operation of UI Render, refer to action_t
    // @return 0 if OK, else fail
    virtual long SetLocalRender(IRtcRender *render, int action) = 0;

    // To set render for remote video, only valid after receiving IRtcSink::OnRemoteStream()
    // @param render: [in] object of UI Render
    // @param action: [in] operation of UI Render, refer to action_t
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
    // @param sdp: [in] sdp of local media (json format, type: offer/pranswer/answer)
    // @return 0 if OK, else fail
    virtual long SetLocalDescription(const std::string &sdp) = 0;

    // To set sdp of remote peer into current peer connection
    // @param sdp: [in] sdp of remote media (json format, type: offer/pranswer/answer)
    // @return 0 if OK, else fail
    virtual long SetRemoteDescription(const std::string &sdp) = 0;

    // To add remote ice candidate into current peer connection
    // @param candidate: [in] ice candidate(json format)
    // @return 0 if OK, else fail
    virtual long AddIceCandidate(const std::string &candidate) = 0;
};


//>
// For C-style interfaces
extern "C" {
bool        xrtc_init();
void        xrtc_uninit();
bool        xrtc_create(IRtcCenter * &prtc);
void        xrtc_destroy(IRtcCenter * prtc);
}


#endif // _XRTC_API_H_

