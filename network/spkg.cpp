#include<condition_variable>
#include<portal/network.h>
#include<json/json.h>
#include<watchdog.h>
#include<functional>
#include<service.h>
#include<string>
#include<vector>
#include<thread>
#include<spkg.h>
#include<map>
#define MODULE_SPKG
#define PKGSEND_LOCK pkglk_send.lock()
#define PKGSEND_UNLOCK pkglk_send.unlock()
#define PKGSEND_POPASSIGN qpkgSend.front(); qpkgSend.pop();
#define PKGSEND_EMPTY (qpkgSend.empty())

#define SPKG_WAITCV unique_lock<mutex> spkg__ulsc(spkg__mtxsc);  pcv_sendchanged.wait(spkg__ulsc)
#define SPKG_WRITEPKG(pkg, clt) clt.Write((char *)&pkg.head, sizeof(pkg_head_t)); clt.Write(pkg.jsdata);
#define SPKG_SENDPKG(pkg, clt) lock_spkg.lock(); clt.Write((char *)&pkg.head, sizeof(pkg_head_t)); \
                               clt.Write(pkg.jsdata); lock_spkg.unlock();
#define SPKG_BIND(msg, func) spkg__func[msg] = func;
#define SPKG_TOFUNCTION(pkg) spkg__func[pkg.head.wopr](pkg);
#define SPKG_GETCLIENT(str) (UserMap[root[str].asString()])

#define JSON_PARSEPKG Json::Value root; Json::Reader reader; reader.parse(pkg.jsdata, root)
#define JSON_GETSTRING(item) (root[item].asString())
#define JSON_GETINT(item) (root[item].asInt())


using namespace std;
using namespace libportal;
EXTERN_PKG_QM
EXTERN_WATCHDOGS
extern map<pkg_t, libportal::TCPClient> UserMap_T;
extern map<string, TCPClient> UserMap;

static map<uint16_t, function<int(pkg_t)> > spkg__func;

condition_variable pcv_sendchanged;
mutex lock_spkg;
mutex spkg__mtxsc;

void spkg_init() {
    cout << "\033[33m[Module SPKG] Loaded!\033[0m\n";

    SPKG_BIND(PT_REG_RES, spkg_special_notlogon);
    SPKG_BIND(PT_RESETPWD_RES, spkg_special_notlogon);

    SPKG_BIND(PT_FL, spkg_to_singleuser);
    SPKG_BIND(PT_GL, spkg_to_singleuser);
    SPKG_BIND(PT_GM, spkg_to_singleuser);
    SPKG_BIND(PT_MUTE_RES, spkg_to_singleuser);
    SPKG_BIND(PT_FETCHCR_RES, spkg_to_singleuser);
    SPKG_BIND(PT_DISMUTE_RES, spkg_to_singleuser);
    SPKG_BIND(PT_CRGRP_RES, spkg_to_singleuser);
    SPKG_BIND(PT_DISGRP_NOT, spkg_to_singleuser);
    SPKG_BIND(PT_PKEY_REQ, spkg_to_singleuser);
    SPKG_BIND(PT_PKEY_RES, spkg_to_singleuser);
    SPKG_BIND(PT_EXGRP_PCNOT, spkg_to_singleuser);
    SPKG_BIND(PT_DELFRI_RES, spkg_to_singleuser);
    SPKG_BIND(PT_FCH_RES, spkg_to_singleuser);

    SPKG_BIND(PT_EXGRP_NOT, spkg_to_group);
    SPKG_BIND(PT_MSG_GRP, spkg_to_group);
    SPKG_BIND(PT_ENGRP_RES, spkg_to_group);
    SPKG_BIND(PT_MGRC_NOT, spkg_to_group);

    SPKG_BIND(PT_ENGRP_PCNOT, spkg_special_pcnot);
    SPKG_BIND(PT_ADDFRI_RES, spkg_special_addfri);
    SPKG_BIND(PT_ADDFRI_REQ, spkg_to_afwho);
    SPKG_BIND(PT_MSG_FRI, spkg_to_eachother);
    SPKG_BIND(PT_LOGIN_RES, spkg_special_login);

    thread spkg_thread(spkg_distribute);
    spkg_thread.join();
}


void spkg_distribute() {
    WHILE_WATCHDOG(SPKG) {
        SPKG_WAITCV;
        PKGSEND_LOCK;
        if (PKGSEND_EMPTY) {
            PKGSEND_UNLOCK;
            continue;
        }
        pkg_t thispkg = PKGSEND_POPASSIGN;

        PKGSEND_UNLOCK;
        cout << "<<< \033[33mSEND:\nOperator:" << thispkg.head.wopr
             << "\nData:" << thispkg.jsdata << "\033[0m\n";
        
        SPKG_TOFUNCTION(thispkg);
    }
}

int spkg_special_pcnot(pkg_t pkg) {
    JSON_PARSEPKG;
    if (JSON_GETINT("res") == 0)
        return 0;

    TCPClient clt = SPKG_GETCLIENT("un");
    SPKG_SENDPKG(pkg, clt);
    return 0;
}

int spkg_special_login(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = (JSON_GETINT("res") == 0) ? SPKG_GETCLIENT("un") : TCPClient(JSON_GETINT("fd"));

    SPKG_SENDPKG(pkg, clt);
    return 0;
}

int spkg_special_notlogon(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = TCPClient(JSON_GETINT("fd"));

    SPKG_SENDPKG(pkg, clt);
    return 0;
}

int spkg_to_singleuser(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = SPKG_GETCLIENT("un");

    SPKG_SENDPKG(pkg, clt);
    return 0;
}

int spkg_to_afwho(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = SPKG_GETCLIENT("afwho");

    SPKG_SENDPKG(pkg, clt);
    return 0;
}

int spkg_to_group(pkg_t pkg) {
    Json::Value gml;
    JSON_PARSEPKG;

    string st;
    srv_getgm(root, st);
    reader.parse(st, gml);

    vector<TCPClient> clts;
    for (auto i = 0u; i < gml["gml"].size(); i++) {
        clts.push_back(UserMap[gml["gml"][i].asString()]);
    }

    lock_spkg.lock();
    for (auto i : clts) {
        SPKG_WRITEPKG(pkg, i);
    }
    lock_spkg.unlock();
    return 0;
}

int spkg_to_eachother(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = SPKG_GETCLIENT("sd");
    SPKG_SENDPKG(pkg, clt);

    clt = SPKG_GETCLIENT("rv");
    SPKG_SENDPKG(pkg, clt);
    return 0;
}


int spkg_special_addfri(pkg_t pkg) {
    JSON_PARSEPKG;
    TCPClient clt = SPKG_GETCLIENT("afwho");
    SPKG_SENDPKG(pkg, clt);

    clt = SPKG_GETCLIENT("un");
    SPKG_SENDPKG(pkg, clt);
    return 0;
}