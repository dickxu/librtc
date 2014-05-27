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
#include "error.h"

static talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pc_factory;

namespace xrtc {

void NavigatorUserMedia::getUserMedia (const MediaStreamConstraints & constraints, NavigatorUserMediaCallback *sink)
{
    return_assert(sink);
    return_assert(_pc_factory.get());

    NavigatorUserMediaError error;

    std::string slabel = "local stream";
    ubase::zeroptr<MediaStream> stream = CreateMediaStream(slabel, _pc_factory, NULL);
    return_assert(stream.get());
    
    // for audio track
    if (constraints.audio) {
        LOGD("get audio track and add it to stream");
        std::string alabel = "local audio track";
        ubase::zeroptr<MediaStreamTrack> audio_track = CreateMediaStreamTrack(XRTC_AUDIO, alabel, NULL, _pc_factory, NULL);
        if (audio_track->getptr() == NULL) {
            LOGW("fail to get audio track")
            error.errstr = "no audio track";
            sink->ErrorCallback(error);
        }else {
            stream->addTrack(audio_track);
        }
    }

    // for video track
    if (constraints.video) {
        LOGD("get video track and add it to stream");
        std::string vlabel = "local video track";
        ubase::zeroptr<MediaStreamTrack> video_track = CreateMediaStreamTrack(XRTC_VIDEO, vlabel, NULL, _pc_factory, NULL);
        if (video_track->getptr() == NULL) {
            LOGW("fail to get video track")
            error.errstr = "no video track";
            sink->ErrorCallback(error);
        }else {
            stream->addTrack(video_track);
        }
    }

    LOGD("return a/v stream")
    sink->SuccessCallback(stream);
}

void GetUserMedia(
        const MediaStreamConstraints & constraints, 
        NavigatorUserMediaCallback *sink,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory)
{
    _pc_factory = pc_factory;
    NavigatorUserMedia::getUserMedia(constraints, sink);
    _pc_factory = NULL;
}

} //namespace xrtc

