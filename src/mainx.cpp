#include "xrtc_api.h"
#include "xrtc_std.h"
#include "webrtc.h"
#include "error.h"


class WebrtcRender : public webrtc::VideoRendererInterface {
private:
    IRtcRender *m_render;
    video_frame_t m_frame;

public:
WebrtcRender() {
    m_render = NULL;
    memset(&m_frame, 0, sizeof(m_frame));
}

virtual ~WebrtcRender() {
    delete m_frame.data;
}

void SetRender(IRtcRender *render) {
    m_render = render;
    if (!render) {
        delete m_frame.data;
        memset(&m_frame, 0, sizeof(m_frame));
    }
}

// For webrtc::VideoRendererInterface
virtual void SetSize(int width, int height) {
    return_assert(m_render);
    m_frame.width = width;
    m_frame.height = height;
    if (m_frame.data == NULL) {
        m_frame.size = width * height * 4;
        m_frame.data = new unsigned char[m_frame.size];
        m_frame.color = ARGB32Fmt;
    }

    m_render->OnSize(width, height);
}

// For webrtc::VideoRendererInterface
virtual void RenderFrame(const cricket::VideoFrame* frame) {
    return_assert(frame);
    return_assert(m_render);
    return_assert(m_frame.data);
    return_assert(m_frame.width == frame->GetWidth());
    return_assert(m_frame.height == frame->GetHeight());

    frame->ConvertToRgbBuffer(cricket::FOURCC_ARGB,
            m_frame.data,
            m_frame.size,
            m_frame.width*4
            );
    m_frame.timestamp = frame->GetTimeStamp();
    m_frame.rotation = frame->GetRotation();
    m_frame.length = m_frame.size;
    m_render->OnFrame(&m_frame);
}

};


class CRtcCenter : public IRtcCenter, 
    public xrtc::NavigatorUserMediaCallback,
    public xrtc::RTCPeerConnectionEventHandler
{
private:
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pc_factory;
    ubase::zeroptr<xrtc::RTCPeerConnection> m_pc;
    ubase::zeroptr<xrtc::MediaStream> m_local_stream;
    IRtcSink *m_sink;
    WebrtcRender *m_local_render;
    WebrtcRender *m_remote_render;

public:
bool Init() {
    m_local_render = new WebrtcRender();
    m_remote_render = new WebrtcRender();
    return true;
}

CRtcCenter() {
    m_pc_factory = NULL;
    m_pc = NULL;
    m_local_stream = NULL;
    m_sink = NULL;
    m_local_render = NULL;
    m_remote_render = NULL;       
}

virtual ~CRtcCenter() {
    delete m_local_render;
    delete m_remote_render;
}

//
// For IRtcCenter
virtual void SetSink(IRtcSink *sink) {
    m_sink = sink;
}

virtual long GetUserMedia(bool has_audio, bool has_video) {
    talk_base::Thread *worker_thread = talk_base::Thread::Current();
    talk_base::Thread *signal_thread = talk_base::Thread::Current(); 
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory = NULL;

    pc_factory = webrtc::CreatePeerConnectionFactory(worker_thread, signal_thread, NULL, NULL, NULL);
    returnv_assert (pc_factory.get(), UBASE_E_FAIL);

    xrtc::MediaStreamConstraints constraints;
    constraints.audio = has_audio;
    constraints.video = has_video;
    xrtc::GetUserMedia(constraints, (xrtc::NavigatorUserMediaCallback *)this, pc_factory);
    return UBASE_S_OK;
}

virtual long CreatePeerConnection() {
    m_pc_factory = webrtc::CreatePeerConnectionFactory();
    returnv_assert (m_pc_factory.get(), UBASE_E_FAIL);

    m_pc = xrtc::CreatePeerConnection(m_pc_factory);
    returnv_assert (m_pc.get(), UBASE_E_FAIL);
    m_pc->Put_EventHandler((xrtc::RTCPeerConnectionEventHandler *)this);
    return UBASE_S_OK;
}

virtual long AddLocalStream() { 
    returnv_assert (m_local_stream.get(), UBASE_E_INVALIDPTR);
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);

    xrtc::MediaConstraints constraints;
    m_pc->addStream(m_local_stream, constraints);
    return UBASE_S_OK;
}

// intenal implemention
long AddRender(sequence<xrtc::MediaStreamPtr> streams, WebrtcRender *render) {
    returnv_assert (!streams.empty(), UBASE_E_FAIL);
    sequence<xrtc::MediaStreamTrackPtr> tracks = streams[0]->getVideoTracks();

    returnv_assert (!tracks.empty(), UBASE_E_FAIL);
    xrtc::VideoStreamTrack *vtrack = (xrtc::VideoStreamTrack *)tracks[0].get();

    webrtc::VideoTrackInterface *mtrack =(webrtc::VideoTrackInterface *) vtrack->getptr();
    returnv_assert (mtrack, UBASE_E_FAIL);
    mtrack->AddRenderer((webrtc::VideoRendererInterface *)render);
    return UBASE_S_OK;
}

long RemoveRender(sequence<xrtc::MediaStreamPtr> streams, WebrtcRender *render) {
    returnv_assert (!streams.empty(), UBASE_E_FAIL);
    sequence<xrtc::MediaStreamTrackPtr> tracks = streams[0]->getVideoTracks();

    returnv_assert (!tracks.empty(), UBASE_E_FAIL);
    xrtc::VideoStreamTrack *vtrack = (xrtc::VideoStreamTrack *)tracks[0].get();

    webrtc::VideoTrackInterface *mtrack =(webrtc::VideoTrackInterface *) vtrack->getptr();
    returnv_assert (mtrack, UBASE_E_FAIL);
    mtrack->RemoveRenderer((webrtc::VideoRendererInterface *)render);
    return UBASE_S_OK;
}

virtual long SetLocalRender(IRtcRender *render, int action) {
    returnv_assert (m_local_render, UBASE_E_INVALIDPTR);
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);

    long lret = UBASE_E_FAIL;
    if (action == ADD_ACTION) {
        returnv_assert (render, UBASE_E_INVALIDARG);
        m_local_render->SetRender(render);
        lret = AddRender(m_pc->getLocalStreams(), m_local_render);
    }else if (action == REMOVE_ACTION){
        lret = RemoveRender(m_pc->getLocalStreams(), m_local_render);
        m_local_render->SetRender(NULL);
    }
    return lret;
}

//
// 1. add flow:
//     PeerConnectionObserver::OnAddStream -> RTCPeerConnectionEventHandler::onaddstream -> 
//     IRtcSink::OnRemoteStream(ADD) -> IRtcCenter::SetRemoteRender(ADD)
// 2. remove flow:
//     PeerConnectionObserver::OnRemoveStream -> RTCPeerConnectionEventHandler::onremovestream -> 
//     IRtcSink::OnRemoteStream(REMOVE) -> IRtcCenter::SetRemoteRender(REMOVE)
virtual long SetRemoteRender(IRtcRender *render, int action) {
    returnv_assert (m_remote_render, UBASE_E_INVALIDPTR);
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);

    long lret = UBASE_E_FAIL;
    if (action == ADD_ACTION) {
        returnv_assert (render, UBASE_E_INVALIDARG);
        m_remote_render->SetRender(render);
        lret = AddRender(m_pc->getRemoteStreams(), m_remote_render);
    }else if (action == REMOVE_ACTION){
        lret = RemoveRender(m_pc->getRemoteStreams(), m_remote_render);
        m_remote_render->SetRender(NULL);
    }
    return lret;
}

virtual long SetupCall() {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaConstraints constraints;
    m_pc->createOffer(constraints);   
    return UBASE_S_OK;
}

virtual long AnswerCall() {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaConstraints constraints;
    m_pc->createAnswer(constraints);   
    return UBASE_S_OK;
}

virtual long SetLocalDescription(const std::string &type, const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCSessionDescription desc;
    desc.type = type;
    desc.sdp = sdp;
    m_pc->setLocalDescription(desc);
    return UBASE_S_OK;
}

virtual long SetRemoteDescription(const std::string & type, const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCSessionDescription desc;
    desc.type = type;
    desc.sdp = sdp;
    m_pc->setRemoteDescription(desc);
    return UBASE_S_OK;
}

virtual long AddIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCIceCandidate ice;
    ice.candidate = candidate;
    ice.sdpMid = sdpMid;
    ice.sdpMLineIndex = sdpMLineIndex;
    m_pc->addIceCandidate(ice);
    return UBASE_S_OK;
}

virtual void Close() {
    return_assert (m_pc.get());
    m_pc->close();
}

//
// For xrtc::NavigatorUserMediaCallback
virtual void SuccessCallback(xrtc::MediaStreamPtr stream)         {
    return_assert(m_sink);
    m_local_stream = stream;
    m_sink->OnGetUserMedia(UBASE_S_OK, "");
}
virtual void ErrorCallback(xrtc::NavigatorUserMediaError &error)  {
    return_assert(m_sink);
    m_sink->OnGetUserMedia(UBASE_E_FAIL, "fail to get local media");
}

//
// For xrtc::RTCPeerConnectionEventHandler
virtual void onnegotiationneeded() {
    return_assert(m_sink);
}
virtual void onicecandidate(xrtc::RTCIceCandidate & ice) {
    return_assert(m_sink);
    m_sink->OnIceCandidate(ice.candidate, ice.sdpMid, ice.sdpMLineIndex);
}
virtual void onsignalingstatechange(int state) {
    return_assert(m_sink);
}
virtual void onaddstream(xrtc::MediaStreamPtr stream) { // remote stream
    return_assert(m_pc.get());
    return_assert(m_sink);

    m_sink->OnRemoteStream(ADD_ACTION);
}
virtual void onremovestream(xrtc::MediaStreamPtr stream) {
    return_assert(m_pc.get());
    return_assert(m_sink);

    m_pc->removeStream(stream);
    m_sink->OnRemoteStream(REMOVE_ACTION);
}
virtual void oniceconnectionstatechange(int state)  {
    return_assert(m_pc.get());
}
virtual void onsuccess(xrtc::RTCSessionDescription &desc) {
    return_assert(m_sink);
    m_sink->OnSessionDescription(desc.type, desc.sdp);
}
virtual void onfailure(const xrtc::DOMString &error) {
    return_assert(m_sink);
    m_sink->OnFailureMesssage("onfailure: " + error);
}
virtual void onerror() {
    return_assert(m_sink);
    m_sink->OnFailureMesssage("onerror: ");
}

};


//
//======================================================

bool xrtc_init()
{
    talk_base::LogMessage::SetDiagnosticMode(true);
    //talk_base::LogMessage::LogToDebug(talk_base::LS_INFO);
    return true;
}

void xrtc_uninit()
{
}

bool xrtc_create(IRtcCenter * &rtc)
{
    rtc = NULL;
    CRtcCenter *pRtc = new CRtcCenter();
    if (!pRtc->Init()) {
        delete pRtc;
        pRtc = NULL;
    }
    rtc = (IRtcCenter *)pRtc;
    return (rtc != NULL);
}

void xrtc_destroy(IRtcCenter *rtc)
{
    return_assert(rtc);
    delete rtc;
}
