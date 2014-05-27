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
#include "observer.h"

namespace xrtc {

//
//> for CRTCPeerConnection
class CRTCPeerConnection : public RTCPeerConnection {
    friend class CRTCPeerConnectionObserver;

private:
    talk_base::scoped_refptr<CRTCPeerConnectionObserver> m_observer;
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> m_conn;

public:
    bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory);

    explicit CRTCPeerConnection ();
    virtual ~CRTCPeerConnection ();
    virtual void * getptr();

    RTCSessionDescription localDescription();
    RTCSessionDescription remoteDescription();
    RTCSignalingState signalingState();
    RTCIceGatheringState iceGatheringState();
    RTCIceConnectionState iceConnectionState();

    virtual void setParams (const RTCConfiguration & configuration, const MediaConstraints & constraints);
    virtual void createOffer (const MediaConstraints & constraints);
    virtual void createAnswer (const MediaConstraints & constraints);
    virtual void setLocalDescription (const RTCSessionDescription & description);
    virtual void setRemoteDescription (const RTCSessionDescription & description);
    virtual void updateIce (const RTCConfiguration & configuration, const MediaConstraints & constraints);
    virtual void addIceCandidate (const RTCIceCandidate & candidate);

    virtual sequence<MediaStreamPtr> getLocalStreams ();
    virtual sequence<MediaStreamPtr> getRemoteStreams ();
    virtual MediaStreamPtr getStreamById (DOMString streamId);

    virtual void addStream (MediaStreamPtr stream, const MediaConstraints & constraints);
    virtual void removeStream (MediaStreamPtr stream);
    virtual void close ();

}; // class CRTCPeerConnection

} 
