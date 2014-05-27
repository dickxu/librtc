#include "observer.h"
#include "peer.h"
#include "error.h"

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
    LOGD("ok");
    event_process0(m_pc, onerror);
}

// Triggered when the SignalingState changed.
void CRTCPeerConnectionObserver::OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) 
{
    //LOGD("ok");
    int state = (int)new_state;
    event_process1(m_pc, onsignalingstatechange, state);
}

// Triggered when SignalingState or IceState have changed.
// TODO(bemasc): Remove once callers transition to OnSignalingChange.
void CRTCPeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) 
{
    //LOGD("ok");
}

// Triggered when media is received on a new stream from remote peer.
void CRTCPeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream) 
{
    LOGD("ok");
    return_assert(stream);
    MediaStreamPtr mstream = CreateMediaStream("", NULL, stream);
    event_process1(m_pc, onaddstream, mstream);
}

// Triggered when a remote peer close a stream.
void CRTCPeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream) 
{
    LOGD("ok");
    return_assert(stream);
    MediaStreamPtr mstream = CreateMediaStream("", NULL, stream);
    event_process1(m_pc, onremovestream, mstream);
}

// Triggered when a remote peer open a data channel.
// TODO(perkj): Make pure virtual.
void CRTCPeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* data_channel) 
{}

// Triggered when renegotation is needed, for example the ICE has restarted.
void CRTCPeerConnectionObserver::OnRenegotiationNeeded() 
{
    //LOGD("ok");
    event_process0(m_pc, onnegotiationneeded);
}

// Called any time the IceConnectionState changes
void CRTCPeerConnectionObserver::OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) 
{
    //LOGD("ok");
    int state = (int)new_state;
    event_process1(m_pc, oniceconnectionstatechange, state);
}

// Called any time the IceGatheringState changes
void CRTCPeerConnectionObserver::OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) 
{
    //LOGD("ok");
}

// New Ice candidate have been found.
void CRTCPeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) 
{
    //LOGD("ok");
    return_assert (candidate);
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        return;
    }

    RTCIceCandidate ice;
    ice.candidate = sdp;
    ice.sdpMid = candidate->sdp_mid();
    ice.sdpMLineIndex = candidate->sdp_mline_index();
    event_process1(m_pc, onicecandidate, ice);
}

// TODO(bemasc): Remove this once callers transition to OnIceGatheringChange.
// All Ice candidates have been found.
void CRTCPeerConnectionObserver::OnIceComplete() {
    //LOGD("ok");
}

///
/// for webrtc::CreateSessionDescriptionObserver
void CRTCPeerConnectionObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
{
    LOGD("ok");
    return_assert(desc);
    RTCSessionDescription rtcdesc;
    rtcdesc.type = desc->type();
    desc->ToString(&rtcdesc.sdp);
    event_process1(m_pc, onsuccess, rtcdesc);
}

void CRTCPeerConnectionObserver::OnFailure(const std::string& error)
{
    LOGD("ok");
    event_process1(m_pc, onfailure, error);
}

}
