#include "webrtc.h"
#include "ubase/error.h"

//
// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

//
// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";


namespace xrtc {
    
bool Convert2Json(const webrtc::SessionDescriptionInterface* description, std::string &json)
{
    if (!description) return false;

    Json::StyledWriter writer;
    Json::Value jmessage;
    jmessage[kSessionDescriptionTypeName] = description->type();
    std::string sdp;
    if (!description->ToString(&sdp)) {
        return false;
    }
    jmessage[kSessionDescriptionSdpName] = sdp;
    json = writer.write(jmessage);
    return true;
}
    
bool Convert2Json(const webrtc::IceCandidateInterface* candidate, std::string &json)
{
    if (!candidate) return false;

    Json::StyledWriter writer;
    Json::Value jmessage;

    jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
    jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        return false;
    }
    jmessage[kCandidateSdpName] = sdp;
    json = writer.write(jmessage);
    return true;
}

bool Convert2SDP(const std::string &json, webrtc::SessionDescriptionInterface* &description)
{
    Json::Reader reader;
    Json::Value jmessage;
    if (!reader.parse(json, jmessage)) {
        return false;
    }

    std::string type;
    std::string sdp;
    GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);
    GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName, &sdp);
    if (type.empty() || sdp.empty()) {
        return false;
    }
        
    description = webrtc::CreateSessionDescription(type, sdp);
    return true;
}

bool Convert2ICE(const std::string &json, webrtc::IceCandidateInterface * &candidate)
{
    Json::Reader reader;
    Json::Value jmessage;
    if (!reader.parse(json, jmessage)) {
        return false;
    }

    std::string sdp_mid;
    int sdp_mlineindex = 0;
    std::string sdp;

    bool bret = false;
    bret = GetStringFromJsonObject(jmessage, kCandidateSdpMidName, &sdp_mid);
    returnb_assert(bret);
    bret = GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp);
    returnb_assert(bret);
    bret = GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName, &sdp_mlineindex);
    returnb_assert(bret);

    candidate = webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp);
    return true;
}
    
}
