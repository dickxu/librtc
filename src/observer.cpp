#include "observer.h"
#include "peer.h"
#include "ubase/error.h"

namespace xrtc {

bool CRTCPeerConnectionObserver::Init(ubase::zeroptr<CRTCPeerConnection> pc, 
        talk_base::scoped_refptr<webrtc::PeerConnectionInterface> conn) 
{
    m_pc = pc;
    m_conn = conn;
    return ((m_pc.get() != NULL) && (m_conn.get() != NULL));
}

CRTCPeerConnectionObserver::CRTCPeerConnectionObserver() 
{
    m_pc = NULL;
    m_conn = NULL;
}

CRTCPeerConnectionObserver::~CRTCPeerConnectionObserver()
{
    m_pc = NULL;
    m_conn = NULL;
}

///
/// for webrtc::PeerConnectionObserver
void CRTCPeerConnectionObserver::OnError() 
{
    event_process0(m_pc, onerror);
}

// Triggered when the SignalingState changed.
void CRTCPeerConnectionObserver::OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) 
{
    int state = (int)new_state;
    event_process1(m_pc, onsignalingstatechange, state);
}

// Triggered when SignalingState or IceState have changed.
// TODO(bemasc): Remove once callers transition to OnSignalingChange.
void CRTCPeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) 
{
    LOGD("from webrtc::PeerConnectionObserver, state_changed="<<state_changed);
}

// Triggered when media is received on a new stream from remote peer.
void CRTCPeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream) 
{
    return_assert(stream);
    
    // Package the stream into MediaStreamPtr which callback for user, e.g. set video render
    MediaStreamPtr mstream = CreateMediaStream("", NULL, stream);
    event_process1(m_pc, onaddstream, mstream);
}

// Triggered when a remote peer close a stream.
void CRTCPeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream) 
{
    return_assert(stream);
    
    // Package the stream into MediaStreamPtr which callback for user, e.g. remove video render
    MediaStreamPtr mstream = CreateMediaStream("", NULL, stream);
    event_process1(m_pc, onremovestream, mstream);
}

// Triggered when a remote peer open a data channel.
// TODO(perkj): Make pure virtual.
void CRTCPeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* data_channel) 
{
    LOGD("from webrtc::PeerConnectionObserver");
}

// Triggered when renegotation is needed, for example the ICE has restarted.
void CRTCPeerConnectionObserver::OnRenegotiationNeeded() 
{
    event_process0(m_pc, onnegotiationneeded);
}

// Called any time the IceConnectionState changes
void CRTCPeerConnectionObserver::OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) 
{
    int state = (int)new_state;
    event_process1(m_pc, oniceconnectionstatechange, state);
}

// Called any time the IceGatheringState changes
void CRTCPeerConnectionObserver::OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) 
{
    LOGD("from webrtc::PeerConnectionObserver, new_state="<<new_state);
}

// New Ice candidate have been found.
void CRTCPeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) 
{
    return_assert (candidate);
    
    std::string json;
    if (Convert2Json(candidate, json)) {
        event_process1(m_pc, onicecandidate, json);
    }
}

// TODO(bemasc): Remove this once callers transition to OnIceGatheringChange.
// All Ice candidates have been found.
void CRTCPeerConnectionObserver::OnIceComplete() {
    LOGD("from webrtc::PeerConnectionObserver");
}

///
/// for webrtc::CreateSessionDescriptionObserver
void CRTCPeerConnectionObserver::OnSuccess(webrtc::SessionDescriptionInterface* description) 
{
    return_assert(description);
    
    std::string json;
    if(Convert2Json(description, json)) {
        event_process1(m_pc, onsuccess, json);
    }
}

void CRTCPeerConnectionObserver::OnFailure(const std::string& error)
{
    event_process1(m_pc, onfailure, error);
}

}
