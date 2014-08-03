#include "webrtc.h"
#include "ubase/error.h"

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
        m_frame.color = kARGB32Fmt;
    }
#if defined(OBJC)
    [m_render OnSize:width height:height];
#else
    m_render->OnSize(width, height);
#endif
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
#if defined(OBJC)
    [m_render OnFrame:&m_frame];
#else
    m_render->OnFrame(&m_frame);
#endif
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

virtual void GetDevices(const device_kind_t kind, devices_t & devices)
{
    devices.clear();
    xrtc::GetDevices(kind, devices);
}

virtual long GetUserMedia(const media_constraints_t & media_constraints) {
    talk_base::Thread *worker_thread = talk_base::Thread::Current();
    talk_base::Thread *signal_thread = talk_base::Thread::Current(); 
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory = NULL;

    pc_factory = webrtc::CreatePeerConnectionFactory(worker_thread, signal_thread, NULL, NULL, NULL);
    returnv_assert (pc_factory.get(), UBASE_E_FAIL);

    xrtc::GetUserMedia(media_constraints, (xrtc::NavigatorUserMediaCallback *)this, pc_factory);
    return UBASE_S_OK;
}

virtual long CreatePeerConnection() {
    ice_server_t server;
    server.uri = xrtc::kDefaultIceServer; // default google stun server

    ice_servers_t servers;
    servers.push_back(server);
    return CreatePeerConnection(servers);
}

virtual long CreatePeerConnection(const ice_servers_t & ice_servers) {
    m_pc_factory = webrtc::CreatePeerConnectionFactory();
    returnv_assert (m_pc_factory.get(), UBASE_E_FAIL);

    webrtc::PeerConnectionInterface::IceServers servers;

    ice_servers_t::const_iterator iter;
    for (iter = ice_servers.begin(); iter != ice_servers.end(); iter++) {
        webrtc::PeerConnectionInterface::IceServer server;
        server.uri = iter->uri;
        server.username = iter->username;
        server.password = iter->password;
        servers.push_back(server);
    }

    m_pc = xrtc::CreatePeerConnection(servers, m_pc_factory);
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
    if (action == kAddStream) {
        returnv_assert (render, UBASE_E_INVALIDARG);
        m_local_render->SetRender(render);
        lret = AddRender(m_pc->getLocalStreams(), m_local_render);
    }else if (action == kRemoveStream){
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
    if (action == kAddStream) {
        returnv_assert (render, UBASE_E_INVALIDARG);
        m_remote_render->SetRender(render);
        lret = AddRender(m_pc->getRemoteStreams(), m_remote_render);
    }else if (action == kRemoveStream){
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

virtual long SetLocalDescription(const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    m_pc->setLocalDescription(sdp);
    return UBASE_S_OK;
}

virtual long SetRemoteDescription(const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    m_pc->setRemoteDescription(sdp);
    return UBASE_S_OK;
}

virtual long AddIceCandidate(const std::string &candidate) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    m_pc->addIceCandidate(candidate);
    return UBASE_S_OK;
}

virtual void Close() {
    if (m_pc.get()) {
        m_pc->close();
        m_pc = NULL;
    }
    m_pc_factory = NULL;
    m_local_stream = NULL;
}

//
// For xrtc::NavigatorUserMediaCallback
virtual void SuccessCallback(xrtc::MediaStreamPtr stream)         {
    return_assert(m_sink);
    m_local_stream = stream;
#if defined(OBJC)
    [m_sink OnGetUserMedia:UBASE_S_OK errstr:""];
#else
    m_sink->OnGetUserMedia(UBASE_S_OK, "");
#endif
}
virtual void ErrorCallback(xrtc::NavigatorUserMediaError &error)  {
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnGetUserMedia:UBASE_E_FAIL errstr:"fail to get local media"];
#else
    m_sink->OnGetUserMedia(UBASE_E_FAIL, "fail to get local media");
#endif
}

//
// For xrtc::RTCPeerConnectionEventHandler
virtual void onnegotiationneeded() {
    return_assert(m_sink);
}
virtual void onicecandidate(const xrtc::DOMString & candidate) {
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnIceCandidate:candidate];
#else
    m_sink->OnIceCandidate(candidate);
#endif
}
virtual void onsignalingstatechange(int state) {
    return_assert(m_sink);
}
virtual void onaddstream(xrtc::MediaStreamPtr stream) { // remote stream
    return_assert(m_pc.get());
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnRemoteStream:kAddStream];
#else
    m_sink->OnRemoteStream(kAddStream);
#endif
}
virtual void onremovestream(xrtc::MediaStreamPtr stream) {
    return_assert(m_pc.get());
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnRemoteStream:kRemoveStream];
#else
    m_sink->OnRemoteStream(kRemoveStream);
#endif
}
virtual void oniceconnectionstatechange(int state)  {
    return_assert(m_pc.get());
#if defined(OBJC)
    [m_sink OnIceConnectionState:state];
#else
    m_sink->OnIceConnectionState(state);
#endif
}
virtual void onsuccess(const xrtc::DOMString &sdp) {
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnSessionDescription:sdp];
#else
    m_sink->OnSessionDescription(sdp);
#endif
}
virtual void onfailure(const xrtc::DOMString &error) {
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnFailure:error];
#else
    m_sink->OnFailure(error);
#endif

}
virtual void onerror() {
    return_assert(m_sink);
#if defined(OBJC)
    [m_sink OnError];
#else
    m_sink->OnError();
#endif

}

};


//
//======================================================

bool xrtc_init()
{
    talk_base::LogMessage::SetDiagnosticMode(true);
    talk_base::LogMessage::LogToDebug(talk_base::LS_INFO);
    talk_base::InitializeSSL();
    return true;
}

void xrtc_uninit()
{
    talk_base::CleanupSSL();
}

bool xrtc_create(IRtcCenter * &prtc)
{
    prtc = new CRtcCenter();
    if (!((CRtcCenter *)prtc)->Init()) {
        delete prtc;
        prtc = NULL;
    }
    return (prtc != NULL);
}

void xrtc_destroy(IRtcCenter *prtc)
{
    return_assert(prtc);
    delete prtc;
}

