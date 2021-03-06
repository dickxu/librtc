#ifndef _MEDIA_CONSTRAINTS_H_
#define _MEDIA_CONSTRAINTS_H_

#include <string>
#include <vector>

#include "talk/app/webrtc/mediaconstraintsinterface.h"
#include "talk/base/stringencode.h"

namespace xrtc {

class WebrtcMediaConstraints : public webrtc::MediaConstraintsInterface {
public:
    WebrtcMediaConstraints() { }
    virtual ~WebrtcMediaConstraints() { }

    WebrtcMediaConstraints & operator = (const WebrtcMediaConstraints &constraints) {
        if (this != &constraints) {
            this->mandatory_ = constraints.mandatory_;
            this->optional_ = constraints.optional_;
        }
        return *this;
    }

    WebrtcMediaConstraints & operator = (const WebrtcMediaConstraints *constraints) {
        if (constraints && this != constraints) {
            this->mandatory_ = constraints->mandatory_;
            this->optional_ = constraints->optional_;
        }
        return *this;
    }

    virtual const Constraints& GetMandatory() const {
        return mandatory_;
    }

    virtual const Constraints& GetOptional() const {
        return optional_;
    }

    template <class T>
    void AddItem(const std::string& key, const T& value, bool optional) {
        if (optional)
            optional_.push_back(Constraint(key, talk_base::ToString<T>(value)));
        else
            mandatory_.push_back(Constraint(key, talk_base::ToString<T>(value)));
    }
    
    template <class T>
    void AddMandatory(const std::string& key, const T& value) {
        mandatory_.push_back(Constraint(key, talk_base::ToString<T>(value)));
    }

    template <class T>
    void SetMandatory(const std::string& key, const T& value) {
        std::string value_str;
        if (mandatory_.FindFirst(key, &value_str)) {
            for (Constraints::iterator iter = mandatory_.begin();
                    iter != mandatory_.end(); ++iter) {
                if (iter->key == key) {
                    mandatory_.erase(iter);
                    break;
                }
            }
        }
        mandatory_.push_back(Constraint(key, talk_base::ToString<T>(value)));
    }

    template <class T>
    void AddOptional(const std::string& key, const T& value) {
        optional_.push_back(Constraint(key, talk_base::ToString<T>(value)));
    }

    void SetMandatoryMinAspectRatio(double ratio) {
        SetMandatory(MediaConstraintsInterface::kMinAspectRatio, ratio);
    }

    void SetMandatoryMinWidth(int width) {
        SetMandatory(MediaConstraintsInterface::kMinWidth, width);
    }

    void SetMandatoryMinHeight(int height) {
        SetMandatory(MediaConstraintsInterface::kMinHeight, height);
    }

    void SetOptionalMaxWidth(int width) {
        AddOptional(MediaConstraintsInterface::kMaxWidth, width);
    }

    void SetMandatoryMaxFrameRate(int frame_rate) {
        SetMandatory(MediaConstraintsInterface::kMaxFrameRate, frame_rate);
    }

    void SetMandatoryReceiveAudio(bool enable) {
        SetMandatory(MediaConstraintsInterface::kOfferToReceiveAudio, enable);
    }

    void SetMandatoryReceiveVideo(bool enable) {
        SetMandatory(MediaConstraintsInterface::kOfferToReceiveVideo, enable);
    }

    void SetMandatoryUseRtpMux(bool enable) {
        SetMandatory(MediaConstraintsInterface::kUseRtpMux, enable);
    }

    void SetMandatoryIceRestart(bool enable) {
        SetMandatory(MediaConstraintsInterface::kIceRestart, enable);
    }

    void SetAllowRtpDataChannels() {
        SetMandatory(MediaConstraintsInterface::kEnableRtpDataChannels, true);
    }

    void SetOptionalVAD(bool enable) {
        AddOptional(MediaConstraintsInterface::kVoiceActivityDetection, enable);
    }

    void SetAllowDtlsSctpDataChannels() {
        SetMandatory(MediaConstraintsInterface::kEnableDtlsSrtp, true);
    }

private:
    Constraints mandatory_;
    Constraints optional_;
};

}  // namespace xrtc

#endif  // _MEDIA_CONSTRAINTS_H_
