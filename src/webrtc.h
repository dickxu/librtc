#ifndef _WEBRTC_H_
#define _WEBRTC_H_

#include "talk/base/common.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/logging.h"
#include "talk/base/physicalsocketserver.h"
#include "talk/media/base/mediaengine.h"
#include "talk/app/webrtc/jsep.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

#include "xrtc_std.h"

namespace xrtc {

void GetUserMedia(
        const MediaStreamConstraints & constraints, 
        NavigatorUserMediaCallback *sink,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory);

ubase::zeroptr<RTCPeerConnection> CreatePeerConnection(
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory);

ubase::zeroptr<MediaStream> CreateMediaStream(
        const std::string label,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamInterface> pstream);

ubase::zeroptr<MediaStreamTrack> CreateMediaStreamTrack(
        media_t mtype,
        const std::string label,
        MediaTrackConstraints *constraints,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> ptrack);

} //namespace xrtc


#endif

