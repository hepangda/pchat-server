#include<vector>
#include<string>
#include<thread>
#include<spkg.h>
#include<map>
#include<json/json.h>
#include<watchdog.h>
#include<functional>
#include<portal/network.h>

using namespace std;
using namespace libportal;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
extern map<string, TCPClient> UserMap;

static map<uint16_t, function<int(pkg_t)> > mPkgfunc;
mutex lock_spkg;

void spkg_init() {

    thread spkg_threads[3];
    for (int i = 0; i < 3; i++) {
        spkg_threads[i] = thread(spkg_distribute);
        spkg_threads[i].detach();
    }
}

void spkg_distribute() {
    while (WATCHDOG_SPKG) {
        pkglk_send.lock();
        pkg_t thispkg = qpkgSend.front();
        qpkgSend.pop();
        pkglk_send.unlock();

        mPkgfunc[thispkg.head.wopr](thispkg);
    }
}

int spkg_to_singleuser(pkg_t pkg) {
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(pkg.jsdata, root);
    TCPClient clt = UserMap[root["un"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write((char *)pkg.jsdata.c_str(), pkg.jsdata.length());
    lock_spkg.unlock();
}