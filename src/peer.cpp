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

#include "peer.h"
#include "error.h"

namespace xrtc {

class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
    static DummySetSessionDescriptionObserver* Create() {
        return new talk_base::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {
    }
    virtual void OnFailure(const std::string& error) {
    }

protected:
    DummySetSessionDescriptionObserver() {}
    ~DummySetSessionDescriptionObserver() {}
};


//
//> for CRTCPeerConnection
bool CRTCPeerConnection::Init(
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory)
{
    returnb_assert (pc_factory.get() != NULL);

    m_observer = new talk_base::RefCountedObject<CRTCPeerConnectionObserver>();

    webrtc::PeerConnectionInterface::IceServers servers;
    webrtc::PeerConnectionInterface::IceServer server;
    server.uri = kDefaultIceServer;
    servers.push_back(server);

    m_conn = pc_factory->CreatePeerConnection(servers, NULL, NULL, (webrtc::PeerConnectionObserver *)m_observer);
    returnb_assert(m_conn.get() != NULL);
    return m_observer->Init(this, m_conn);
}
 
CRTCPeerConnection::CRTCPeerConnection ()
{
    m_conn = NULL;
    m_observer = NULL; 
}

CRTCPeerConnection::~CRTCPeerConnection ()
{
    m_conn = NULL;
    m_observer = NULL;
}

void * CRTCPeerConnection::getptr() 
{
    return m_conn.get();
}

RTCSessionDescription CRTCPeerConnection::localDescription() 
{
    RTCSessionDescription rtcdesc;
    returnv_assert(m_conn.get(), rtcdesc);

    const webrtc::SessionDescriptionInterface *desc = m_conn->local_description();
    if (desc) {
        rtcdesc.type = desc->type();
        desc->ToString(&rtcdesc.sdp);
    }
    return rtcdesc;
}

RTCSessionDescription CRTCPeerConnection::remoteDescription() 
{
    RTCSessionDescription rtcdesc;
    returnv_assert(m_conn.get(), rtcdesc);

    const webrtc::SessionDescriptionInterface *desc = m_conn->remote_description();
    if (desc) {
        rtcdesc.type = desc->type();
        desc->ToString(&rtcdesc.sdp);
    }
    return rtcdesc;
}

RTCSignalingState CRTCPeerConnection::signalingState() 
{
    RTCSignalingState state = SIGNALING_CLOSED;
    returnv_assert(m_conn.get(), state);
    state = (RTCSignalingState) m_conn->signaling_state();
    return state;
}

RTCIceGatheringState CRTCPeerConnection::iceGatheringState() 
{
    RTCIceGatheringState state = ICE_NEW;
    returnv_assert(m_conn.get(), state);
    state = (RTCIceGatheringState) m_conn->ice_gathering_state();
    return state;
}

RTCIceConnectionState CRTCPeerConnection::iceConnectionState() 
{
    RTCIceConnectionState state = CONN_NEW;
    returnv_assert(m_conn.get(), state);
    state = (RTCIceConnectionState) m_conn->ice_connection_state();
    return state;
}

void CRTCPeerConnection::setParams (const RTCConfiguration & configuration, const MediaConstraints & constraints)
{}

void CRTCPeerConnection::createOffer (const MediaConstraints & constraints)
{
    return_assert(m_conn.get());
    return_assert(m_observer.get());
    m_conn->CreateOffer((webrtc::CreateSessionDescriptionObserver *)m_observer, NULL);
}

void CRTCPeerConnection::createAnswer (const MediaConstraints & constraints)
{
    return_assert(m_conn.get());
    return_assert(m_observer.get());
    m_conn->CreateAnswer((webrtc::CreateSessionDescriptionObserver *)m_observer, NULL);
}

void CRTCPeerConnection::setLocalDescription (const RTCSessionDescription & description)
{
    return_assert(m_conn.get());
    webrtc::SessionDescriptionInterface* desp(webrtc::CreateSessionDescription(description.type, description.sdp));
    if (desp) {
        m_conn->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desp);
    }
}

void CRTCPeerConnection::setRemoteDescription (const RTCSessionDescription & description)
{
    return_assert(m_conn.get());
    webrtc::SessionDescriptionInterface* desp(webrtc::CreateSessionDescription(description.type, description.sdp));
    if (desp) {
        m_conn->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desp);
    }
}

void CRTCPeerConnection::updateIce (const RTCConfiguration & configuration, const MediaConstraints & constraints)
{
}

void CRTCPeerConnection::addIceCandidate (const RTCIceCandidate & ice)
{
    return_assert(m_conn.get());
    talk_base::scoped_ptr<webrtc::IceCandidateInterface> candidate(
            webrtc::CreateIceCandidate(ice.sdpMid, ice.sdpMLineIndex, ice.candidate));
    m_conn->AddIceCandidate(candidate.get());
}

sequence<MediaStreamPtr> CRTCPeerConnection::getLocalStreams ()
{
    sequence<MediaStreamPtr> streams;
    returnv_assert(m_conn.get(), streams);

    talk_base::scoped_refptr<webrtc::StreamCollectionInterface> collection = m_conn->local_streams();
    returnv_assert(collection.get(), streams);
    for (size_t k=0; k < collection->count(); k++) {
        webrtc::MediaStreamInterface *pstream = collection->at(k);
        MediaStreamPtr stream = CreateMediaStream("", NULL, pstream);
        streams.push_back(stream);
    }

    return streams;
}

sequence<MediaStreamPtr> CRTCPeerConnection::getRemoteStreams ()
{
    sequence<MediaStreamPtr> streams;
    returnv_assert(m_conn.get(), streams);

    talk_base::scoped_refptr<webrtc::StreamCollectionInterface> collection = m_conn->remote_streams();
    returnv_assert(collection.get(), streams);
    for (size_t k=0; k < collection->count(); k++) {
        webrtc::MediaStreamInterface *pstream = collection->at(k);
        MediaStreamPtr stream = CreateMediaStream("", NULL, pstream);
        if (stream)
            streams.push_back(stream);
    }
    return streams;
}

MediaStreamPtr CRTCPeerConnection::getStreamById (DOMString streamId)
{
    MediaStreamPtr stream;
    returnv_assert(m_conn.get(), stream);

    talk_base::scoped_refptr<webrtc::StreamCollectionInterface> collection = NULL;
    collection = m_conn->local_streams();
    if (collection) {
        webrtc::MediaStreamInterface* pstream = collection->find(streamId);
        MediaStreamPtr stream = CreateMediaStream("", NULL, pstream);
        if (stream)
            return stream;
    }

    collection = m_conn->remote_streams();
    if (collection) {
        webrtc::MediaStreamInterface* pstream = collection->find(streamId);
        MediaStreamPtr stream = CreateMediaStream("", NULL, pstream);
        if (stream)
            return stream;
    }

    return NULL;
}

void CRTCPeerConnection::addStream (MediaStreamPtr stream, const MediaConstraints & constraints)
{
    return_assert(m_conn.get());
    if (stream && stream->getptr()) {
        webrtc::MediaConstraintsInterface* constraints = NULL;
        bool bret = m_conn->AddStream((webrtc::MediaStreamInterface *)stream->getptr(), constraints);
        LOGI("Add stream to PeerConnection success="<<bret);
    }
}

void CRTCPeerConnection::removeStream (MediaStreamPtr stream)
{
    return_assert(m_conn.get());
    if (stream && stream->getptr()) {
        m_conn->RemoveStream((webrtc::MediaStreamInterface *)stream->getptr());
    }
}

void CRTCPeerConnection::close ()
{
    return_assert(m_conn.get());
    m_conn->Close();
}


//
//> for create interface
ubase::zeroptr<RTCPeerConnection> CreatePeerConnection(
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory) {
    ubase::zeroptr<CRTCPeerConnection> pc = new ubase::RefCounted<CRTCPeerConnection>();
    if (!pc.get() || !pc->Init(pc_factory)) {
        pc = NULL;
    }
    return pc;
}


} //namespace xrtc
