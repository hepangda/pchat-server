#include<vector>
#include<string>
#include<thread>
#include<condition_variable>
#include<spkg.h>
#include<map>
#include<json/json.h>
#include<watchdog.h>
#include<functional>
#include<portal/network.h>
#include<service.h>
using namespace std;
using namespace libportal;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
extern map<string, TCPClient> UserMap;
static map<uint16_t, function<int(pkg_t)> > mPkgfunc;

mutex pmtx_sendchanged;
condition_variable pcv_sendchanged;

mutex lock_spkg;

void spkg_init() {
    cout << "[Module SPKG] Loaded!" << endl;
    mPkgfunc[PT_LOGIN_RES] = spkg_to_singleuser;
    mPkgfunc[PT_REG_RES] = spkg_to_singleuser;
    mPkgfunc[PT_RESETPWD_RES] = spkg_to_singleuser;
    mPkgfunc[PT_FL] = spkg_to_singleuser;
    mPkgfunc[PT_GL] = spkg_to_singleuser;
    mPkgfunc[PT_GM] = spkg_to_singleuser;
    mPkgfunc[PT_MUTE_RES] = spkg_to_singleuser;
    mPkgfunc[PT_FETCHCR_RES] = spkg_to_singleuser;//TODO:
    mPkgfunc[PT_DISMUTE_RES] = spkg_to_singleuser;
    mPkgfunc[PT_CRGRP_RES] = spkg_to_singleuser;
    mPkgfunc[PT_DISGRP_NOT] = spkg_to_singleuser; //需要给所有人都发送，所以要一次性写入很多个包
    mPkgfunc[PT_PKEY_REQ] = spkg_to_singleuser;   //TODO:
    mPkgfunc[PT_PKEY_RES] = spkg_to_singleuser;   //TODO:
    mPkgfunc[PT_EXGRP_PCNOT] = spkg_to_singleuser;
    mPkgfunc[PT_DELFRI_RES] = spkg_to_singleuser;

    mPkgfunc[PT_EXGRP_NOT] = spkg_to_group;
    mPkgfunc[PT_MSG_GRP] = spkg_to_group;
    mPkgfunc[PT_ENGRP_RES] = spkg_to_group;
    mPkgfunc[PT_MGRC_NOT] = spkg_to_group;

    mPkgfunc[PT_ADDFRI_REQ] = spkg_to_afwho;
    mPkgfunc[PT_MSG_FRI] = spkg_to_eachother;//TODO:
    mPkgfunc[PT_ADDFRI_RES] = spkg_special_addfri;
    // mPkgfunc[PT_ONLINE] = ; //TODO:
    // mPkgfunc[PT_OFFLINE] = ;//TODO:

    thread spkg_thread(spkg_distribute);
    spkg_thread.join();
}

void spkg_distribute() {
    while (WATCHDOG_SPKG) {
        unique_lock<mutex> pul_sendchanged(pmtx_sendchanged);
        pcv_sendchanged.wait(pul_sendchanged);
        pkglk_send.lock();
        if (qpkgSend.empty()) {
            pkglk_send.unlock();
            continue;
        }
        pkg_t thispkg = qpkgSend.front();
        qpkgSend.pop();
        pkglk_send.unlock();
        cout << "spkg a pack=#" << thispkg.jsdata << "#" << endl;
        mPkgfunc[thispkg.head.wopr](thispkg);
    }
}

int spkg_to_singleuser(pkg_t pkg) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(pkg.jsdata, root);
    cout << "here" << endl;
    TCPClient clt = UserMap[root["un"].asString()];
    cout << clt.getfd() << endl;
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();
    return 0;
}

int spkg_to_afwho(pkg_t pkg) {
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(pkg.jsdata, root);
    TCPClient clt = UserMap[root["afwho"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();
    return 0;
}

int spkg_to_group(pkg_t pkg) {
    Json::Value root, gml;
    Json::Reader reader;
    string st;

    reader.parse(pkg.jsdata, root);
    srv_getgm(root, st);
    reader.parse(st, gml);

    vector<TCPClient> clts;
    for (auto i = 0u; i < gml["gml"].size(); i++) {
        clts.push_back(UserMap[gml["gml"][i].asString()]);
    }

    lock_spkg.lock();
    for (auto i : clts) {
        i.Write((char *)&pkg.head, sizeof(pkg_head_t));
        i.Write(pkg.jsdata);
    }
    lock_spkg.unlock();
    return 0;
}

int spkg_to_eachother(pkg_t pkg) {
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(pkg.jsdata, root);
    TCPClient clt = UserMap[root["sd"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();

    reader.parse(pkg.jsdata, root);
    clt = UserMap[root["rv"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();
    return 0;
}

int spkg_special_addfri(pkg_t pkg) {
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(pkg.jsdata, root);
    TCPClient clt = UserMap[root["afwho"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();

    reader.parse(pkg.jsdata, root);
    clt = UserMap[root["un"].asString()];
    lock_spkg.lock();
    clt.Write((char *)&pkg.head, sizeof(pkg_head_t));
    clt.Write(pkg.jsdata);
    lock_spkg.unlock();
    return 0;
}