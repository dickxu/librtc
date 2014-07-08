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

#ifndef _XRTC_STD_H_
#define _XRTC_STD_H_

#include "xrtc_common.h"
#include "ubase/refcount.h"
#include "ubase/zeroptr.h"

namespace xrtc {


class EventTarget : public ubase::RefCount {
public:
    virtual ~EventTarget() {}
    virtual void * getptr()         {return NULL;}
};

#define readonly_attribute(t, n)        protected: public: virtual t n() = 0;
#define readwrite_attribute(t, n)       public:    virtual void Put_##n(t value) = 0; public: virtual t n() = 0;
#define eventhandler_attribute(c)       public: virtual void Put_EventHandler(c##EventHandler *handler) { m_pEventHandler = handler; } \
                                        protected: virtual c##EventHandler * Get_EventHandler() {return m_pEventHandler; } \
                                        protected: c##EventHandler *m_pEventHandler;
#define event_process0(h, f)            if(h && (h)->Get_EventHandler()) { (h)->Get_EventHandler()->f(); }
#define event_process1(h, f, a)         if(h && (h)->Get_EventHandler()) { (h)->Get_EventHandler()->f((a)); }
#define event_process2(h, f, a, b)      if(h && (h)->Get_EventHandler()) { (h)->Get_EventHandler()->f((a), (b)); }


//
// For Media Stream Track
class MediaStreamTrackEventHandler {
public:
    virtual void onmute()       {}
    virtual void onunmute()     {}
    virtual void onstarted()    {}
    virtual void onended()      {}
    virtual void onoverconstrained()    {}
};

class MediaStreamTrack : public EventTarget {
    readonly_attribute (DOMString,             kind);
    readonly_attribute (DOMString,             id);
    readonly_attribute (DOMString,             label);
    readwrite_attribute (boolean,              enabled);
    readonly_attribute (boolean,               muted);
    //readonly_attribute (boolean,               readonly);
    //readonly_attribute (boolean,               remote);
    readonly_attribute (MediaStreamTrackState, readyState);

    eventhandler_attribute (MediaStreamTrack);

public:
    explicit MediaStreamTrack() {m_pEventHandler = NULL;}
    virtual ~MediaStreamTrack() {}

    virtual MediaTrackConstraints   constraints ()      = 0;
    virtual MediaSourceStates       states ()           = 0;
    virtual AllMediaCapabilities *  capabilities ()     = 0;
    virtual void                    applyConstraints (MediaTrackConstraints &constraints) {}
    //virtual MediaStreamTrack        clone () {}
    virtual void                    stop () {}
};
typedef ubase::zeroptr<MediaStreamTrack> MediaStreamTrackPtr;
//typedef sequence<MediaStreamTrackPtr> MediaStreamTrackSequence;

class VideoStreamTrack : public MediaStreamTrack {
public:
    static sequence<SourceInfo> & getSourceInfos();
};

class AudioStreamTrack : public MediaStreamTrack {
public:
    static sequence<SourceInfo> & getSourceInfos();
};


//
// For Media Stream
class MediaStreamEventHandler {
public:
    virtual void onended()          {}
    virtual void onaddtrack()       {}
    virtual void onremovetrack()    {}
};

class MediaStream : public EventTarget {
    readonly_attribute (DOMString,    id);
    readonly_attribute (boolean,      ended);

    eventhandler_attribute (MediaStream);

public:
    explicit MediaStream () {m_pEventHandler = NULL;}
    virtual ~MediaStream () {}

    virtual sequence<MediaStreamTrackPtr> getAudioTracks ()           = 0;
    virtual sequence<MediaStreamTrackPtr> getVideoTracks ()           = 0;
    virtual MediaStreamTrackPtr      getTrackById (DOMString trackId)   = 0;
    virtual void                     addTrack (MediaStreamTrackPtr track) {}
    virtual void                     removeTrack (MediaStreamTrackPtr track) {}
    //virtual MediaStream              clone () {}
};
typedef ubase::zeroptr<MediaStream> MediaStreamPtr;


//
// For Local Stream of a/v device
class NavigatorUserMediaCallback {
public:
    virtual void SuccessCallback(MediaStreamPtr stream)         {};
    virtual void ErrorCallback(NavigatorUserMediaError &error)  {};
};

class NavigatorUserMedia {
public:
    static void getUserMedia (const MediaStreamConstraints & constraints, NavigatorUserMediaCallback *callback);
};


//
// For Peer Connection
class RTCPeerConnectionEventHandler {
public:
    virtual void onnegotiationneeded()                  {}
    virtual void onicecandidate(const DOMString & candidate)  {}
    virtual void onsignalingstatechange(int state)      {}
    virtual void onaddstream(MediaStreamPtr)            {}
    virtual void onremovestream(MediaStreamPtr)         {}
    virtual void oniceconnectionstatechange(int state)  {}

    virtual void onsuccess(const DOMString &sdp)        {}
    virtual void onfailure(const DOMString &error)      {}
    virtual void onerror()                              {}
};

class RTCPeerConnection : public EventTarget  {
    readonly_attribute (DOMString,  localDescription);
    readonly_attribute (DOMString,  remoteDescription);
    readonly_attribute (RTCSignalingState,      signalingState);
    readonly_attribute (RTCIceGatheringState,   iceGatheringState);
    readonly_attribute (RTCIceConnectionState,  iceConnectionState);

    eventhandler_attribute (RTCPeerConnection);

public:
    explicit RTCPeerConnection () {m_pEventHandler=NULL;}
    virtual ~RTCPeerConnection () {}

    virtual void setParams (const RTCConfiguration & configuration, const MediaConstraints & constraints) {}
    virtual void createOffer (const MediaConstraints & constraints) = 0;
    virtual void createAnswer (const MediaConstraints & constraints) = 0;
    virtual void setLocalDescription (const DOMString & description) = 0;
    virtual void setRemoteDescription (const DOMString & description) = 0;

    virtual void updateIce (const RTCConfiguration & configuration, const MediaConstraints & constraints) {}
    virtual void addIceCandidate (const DOMString & candidate) = 0;

    virtual sequence<MediaStreamPtr> getLocalStreams ()         = 0;
    virtual sequence<MediaStreamPtr> getRemoteStreams ()        = 0;
    virtual MediaStreamPtr getStreamById (DOMString streamId)   = 0;

    virtual void addStream (MediaStreamPtr stream, const MediaConstraints & constraints) = 0;
    virtual void removeStream (MediaStreamPtr stream) = 0;
    virtual void close () = 0;
};
typedef ubase::zeroptr<RTCPeerConnection> RTCPeerConnectionPtr;

} // namespace xrtc


#endif // _XRTC_STD_H_

