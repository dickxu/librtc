#include "xrtc_api.h"
#include "error.h"
#include "pclient.h"
#include "mutex.h"

#include <sstream>
#include <unistd.h>

static std::string kServIp = "127.0.0.1";
static int kServPort = 8888;

struct Node{
    std::string name;
    int id;
};
static Node kMyNode = {"myself", -1};
static Node kPeerNode = {"peer", -1};

enum {
    ON_NONE,
    ON_LOGIN,
    ON_SEND,
};

class CustomThread {
public:
    CustomThread() : quit_(false) {}
    virtual ~CustomThread() {quit_=true;}
    void Start() {
        pthread_t tid;
        quit_ = false;
        pthread_create(&tid, NULL, CustomThread::Entry, (void *)this);
    }
    void Stop() { quit_ = true; usleep(1000);}
    virtual bool Wait(int cms, bool process_io) = 0;

protected:
    static void * Entry(void *param) {
        CustomThread *thiz = (CustomThread *)param;
        if (!thiz) return NULL;
        thiz->Run();
        return NULL;
    }
    void Run() {
        do { Wait(100, false); }while(!quit_); 
    }
    bool quit_;
};

class CRtcRender : public IRtcRender {
private:
    std::string m_tag;

public:
    explicit CRtcRender(std::string tag) {
        m_tag = "[" + tag + "]";
    }
    virtual ~CRtcRender() {}
    virtual void OnSize(int width, int height) {
        LOGI(m_tag<<" width="<<width<<", height="<<height);
    }
    virtual void OnFrame(const video_frame_t *frame) {
        LOGI(m_tag<<" length="<<frame->length);
    }
};

class CEventServer : public talk_base::PhysicalSocketServer {
public:
    CEventServer(talk_base::Thread* thread, PeerConnectionClient *client) : client_(client), init_(false) {Init();}
    virtual ~CEventServer() {Uninit();}
    bool Init() {
        if (init_) return true;
        init_ = true;
        return true;
    }
    void Uninit() {
        if (!init_) return;
        init_ = false;
    }
    void PostMsg(int id, std::string msg) { 
        ubase::ScopedLock lock(mtx_); 
        msgs_.push(std::pair<int, std::string>(id, msg));
    }

    // Override so that we can also pump the GTK message loop.
    virtual bool Wait(int cms, bool process_io) {
        do {
            ubase::ScopedLock lock(mtx_);
            if (msgs_.empty()) break;
            std::pair<int, std::string> &msg = msgs_.front();
            switch(msg.first) {
            case ON_LOGIN:
                client_->Connect(kServIp, kServPort, kMyNode.name);
                client_->OnMessage(NULL);
                break;
            case ON_SEND:
                client_->SendToPeer(kPeerNode.id, msg.second);
                break;
            }
            msgs_.pop();
        }while(false);
        usleep(100*1000);
        return talk_base::PhysicalSocketServer::Wait(0, process_io);
    }

protected:
    talk_base::Thread* thread_;
    PeerConnectionClient* client_;
    bool init_;
    ubase::Mutex mtx_;
    std::queue<std::pair<int, std::string> > msgs_;
};

class CApplication : public IRtcSink, public PeerConnectionClientObserver {
private:
    IRtcCenter *m_rtc;
    CEventServer *m_event;
    IRtcRender *m_rrender;

public:
    CApplication(IRtcCenter *rtc) : m_rtc(rtc) {
        m_rrender = new CRtcRender("remote render");
    }
    virtual ~CApplication() {
        delete m_rrender;
    }
    void SetEvent(CEventServer *event) {m_event=event;}

    virtual void OnSessionDescription(const std::string &type, const std::string &sdp) {
        LOGI("type="<<type<<",sdp=...");
        m_rtc->SetLocalDescription(type, sdp);

        std::stringstream stream;
        stream << "type: " << type << "||";
        stream << "sdp: " << sdp << "||";
        //LOGI("msg="<<stream.str());
        m_event->PostMsg(ON_SEND, stream.str());
    }
    virtual void OnIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) {
        LOGI("candidate="<<candidate<<",sdpMid="<<sdpMid<<",sdpMLineIndex="<<sdpMLineIndex);

        std::stringstream stream;
        stream << "sdpMid: " << sdpMid << "||";
        stream << "sdpMLineIndex: " << sdpMLineIndex << "||";
        stream << "candidate: " << candidate << "||";
        //LOGI("msg="<<stream.str());
        m_event->PostMsg(ON_SEND, stream.str());
    }
    //> action: refer to action_t
    virtual void OnRemoteStream(int action) {
        m_rtc->SetRemoteRender(m_rrender, action);
    }
    virtual void OnGetUserMedia(int error, std::string errstr) {
        LOGI("error="<<error<<", errstr="<<errstr);
    }
    virtual void OnFailureMesssage(std::string errstr) {
        LOGI("msg="<<errstr);
    }
    virtual void OnSignedIn() {
        LOGD("ok");
    }
    virtual void OnDisconnected() {
        LOGD("ok");
    }
    virtual void OnPeerConnected(int id, const std::string& name) {
        LOGD("id="<<id<<", name="<<name);
    }
    virtual void OnPeerDisconnected(int peer_id) {
        LOGD("peer_id="<<peer_id);
    }
    virtual void OnMessageFromPeer(int peer_id, const std::string& message) {
        LOGD("peer_id="<<peer_id); //<<", message="<<message);
        kPeerNode.id = peer_id;

        std::string type;
        int pos1 = message.find("type: ");
        int pos2 = message.find("||", pos1+6);
        if (pos1 != -1 && pos2 != -1) {
            type = message.substr(pos1+6, pos2-pos1-6);
        }
        
        if (!type.empty()) {
            std::string sdp;
            pos1 = message.find("sdp: ");
            pos2 = message.find("||", pos1+5);
            if (pos1 == -1 || pos2 == -1) return;
            sdp = message.substr(pos1+5, pos2-pos1-5);

            LOGD("type="<<type<<", if offer will do CreateAnswer");
            m_rtc->SetRemoteDescription(type, sdp);
            if (type == "offer") {
                m_rtc->AnswerCall();
            }
        } else {
            std::string sdp_mid;
            int sdp_mlineindex = 0;
            std::string sdp;
            
            pos1 = message.find("sdpMid: ");
            pos2 = message.find("||", pos1+8);
            if (pos1 == -1 || pos2 == -1) return;
            sdp_mid = message.substr(pos1+8, pos2-pos1-8);
            pos1 = message.find("sdpMLineIndex: ");
            pos2 = message.find("||", pos1+15);
            if (pos1 == -1 || pos2 == -1) return;
            sdp_mlineindex = atoi(message.substr(pos1+15, pos2-pos1-15).c_str());
            pos1 = message.find("candidate: ");
            pos2 = message.find("||", pos1+11);
            if (pos1 == -1 || pos2 == -1) return;
            sdp = message.substr(pos1+11, pos2-pos1-11);
                
            LOGD("sdp_mid="<<sdp_mid<<", sdp_mlineindex="<<sdp_mlineindex);
            m_rtc->AddIceCandidate(sdp, sdp_mid, sdp_mlineindex);
        }
    }
    virtual void OnMessageSent(int err) {
        LOGD("err="<<err);
    }
    virtual void OnServerConnectionFailure() {
        LOGD("ok");
    }
};

void usage() {
    const char *kUsage =
        "h: help\n"
        "l: login\n"
        "g: GetUserMedia\n"
        "c: CreatePeerConnection\n"
        "s: SetupCall\n" 
        "q: quit\n"
    ;
    std::cout<<kUsage<<std::endl;
}

int main(int argc, char *argv[]) {
    xrtc_init();

    IRtcCenter *rtc = NULL;
    xrtc_create(rtc);

    CApplication *app = new CApplication(rtc);
    rtc->SetSink((IRtcSink *)app);

    PeerConnectionClient *client = new PeerConnectionClient();
    client->RegisterObserver((PeerConnectionClientObserver*)app);

    talk_base::Thread* thread = new talk_base::Thread();
    CEventServer* event = new CEventServer(thread, client);
    app->SetEvent(event);
    thread->set_socketserver(event);
    thread->Start();

    IRtcRender *lrender = new CRtcRender("local render");

    bool quit = false;
    do {
        printf(">");
        char ch = getchar();
        switch(ch) {
        case 'h': usage(); break;
        case 'l':
            LOGD("login server ...");
            //std::cout<<"IP: "; std::cin>>kServIp;
            //std::cout<<"Port: "; std::cin>>kServPort;
            std::cout<<"My Name: "; std::cin>>kMyNode.name;
            event->PostMsg(ON_LOGIN, "");
            break;
        case 'c': 
            rtc->CreatePeerConnection();
            break;
        case 'g': 
            rtc->GetUserMedia(true, true);
            break;
        case 's': 
            LOGD("call peer ...");
            std::cout<<"Peer Id: "; std::cin>>kPeerNode.id;
            rtc->AddLocalStream();
            rtc->SetLocalRender(lrender, ADD_ACTION);
            rtc->SetupCall();
            break;
        case 'q': quit=true; break;
        }
    }while(!quit);

    thread->Quit();
    delete app;
    delete client;
    xrtc_destroy(rtc);
    xrtc_uninit();
    return 0;
}

