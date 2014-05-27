#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include "webrtc.h"


namespace xrtc {

class CRTCPeerConnection;

//
//> for CRTCPeerConnectionObserver
class CRTCPeerConnectionObserver :
    public webrtc::PeerConnectionObserver,
    public webrtc::CreateSessionDescriptionObserver {
private:
    ubase::zeroptr<CRTCPeerConnection> m_pc;
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> m_conn;

public:
    bool Init(ubase::zeroptr<CRTCPeerConnection> pc, talk_base::scoped_refptr<webrtc::PeerConnectionInterface> conn);
    explicit CRTCPeerConnectionObserver();
    virtual ~CRTCPeerConnectionObserver();

    ///
    /// for webrtc::PeerConnectionObserver
    virtual void OnError() ;

    // Triggered when the SignalingState changed.
    virtual void OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) ;

    // Triggered when SignalingState or IceState have changed.
    // TODO(bemasc): Remove once callers transition to OnSignalingChange.
    virtual void OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) ;

    // Triggered when media is received on a new stream from remote peer.
    virtual void OnAddStream(webrtc::MediaStreamInterface* stream) ;

    // Triggered when a remote peer close a stream.
    virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream) ;

    // Triggered when a remote peer open a data channel.
    // TODO(perkj): Make pure virtual.
    virtual void OnDataChannel(webrtc::DataChannelInterface* data_channel) ;

    // Triggered when renegotation is needed, for example the ICE has restarted.
    virtual void OnRenegotiationNeeded() ;

    // Called any time the IceConnectionState changes
    virtual void OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) ;

    // Called any time the IceGatheringState changes
    virtual void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) ;

    // New Ice candidate have been found.
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) ;

    // TODO(bemasc): Remove this once callers transition to OnIceGatheringChange.
    // All Ice candidates have been found.
    virtual void OnIceComplete();

    ///
    /// for webrtc::CreateSessionDescriptionObserver
    virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) ;
    virtual void OnFailure(const std::string& error) ;

}; 

}

#endif // _OBSERVER_H_
