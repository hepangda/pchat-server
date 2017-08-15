#include<service.h>
#include<dpkg.h>
#include<watchdog.h>
#include<queue>
#include<mutex>
#include<map>
#include<thread>
#include<functional>
#include<condition_variable>
#include<portal/network.h>
using namespace std;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;

static map<uint16_t, function<int(pkg_t)> > mPkgfunc;
extern map<string, libportal::TCPClient> UserMap;
extern map<pkg_t, libportal::TCPClient> UserMap_T;
extern condition_variable pcv_sendchanged;

mutex pmtx_recvchanged;
condition_variable pcv_recvchanged;


void dpkg_init() {
    cout << "\n[Module DPKG] Loaded!\n";
    //登录相关请求
    mPkgfunc[PT_LOGIN_REQ] = dpkg_login_request;
    mPkgfunc[PT_REG_REQ] = dpkg_register_request;
    mPkgfunc[PT_RESETPWD_REQ] = dpkg_resetpwd_request;

    //好友相关请求
    mPkgfunc[PT_ADDFRI_REQ] = dpkg_addfriend_request;
    mPkgfunc[PT_DELFRI_REQ] = dpkg_delfriend_request;
    mPkgfunc[PT_ADDFRI_APP] = dpkg_addfriend_application;
    mPkgfunc[PT_REFRESH_FL] = dpkg_refreshfl_request;
    mPkgfunc[PT_REFRESH_GL] = dpkg_refreshgl_request;
    mPkgfunc[PT_MUTE_REQ] = dpkg_mute_request;
    mPkgfunc[PT_DISMUTE_REQ] = dpkg_dismute_request;
    // mPkgfunc[PT_MSG_FRI] = ;

    //群相关请求
    mPkgfunc[PT_REFRESH_GM] = dpkg_refreshgm_request;
    mPkgfunc[PT_CRGRP_REQ] = dpkg_creategroup_request;
    mPkgfunc[PT_DISGRP_REQ] = dpkg_dismissgroup_request;
    mPkgfunc[PT_EXGRP_REQ] = dpkg_exitgroup_request;
    mPkgfunc[PT_ENGRP_REQ] = dpkg_entergroup_request;
    mPkgfunc[PT_SCMGR_REQ] = dpkg_scmgr_request;
    // mPkgfunc[PT_MSG_GRP] = ;

    // mPkgfunc[PT_ONLINE] = ;
    // mPkgfunc[PT_OFFLINE] = ;
    // mPkgfunc[PT_PKEY_REQ] = ;
    // mPkgfunc[PT_PKEY_RES] = ;
    // mPkgfunc[PT_FETCHCR_REQ] = ;

    thread dpkg_threads[3];
    for (int i = 0; i < 3; i++) {
        dpkg_threads[i] = thread(dpkg_distribute);
    }

    for (int i = 0; i < 3; i++) {
        dpkg_threads[i].join();
    }
}

void dpkg_distribute() {
    while (WATCHDOG_DPKG) {
        unique_lock<mutex> pul_recvchanged(pmtx_recvchanged);
        pcv_recvchanged.wait(pul_recvchanged);
        pkglk_recv.lock();
        if (qpkgRecv.empty()) {
            pkglk_recv.unlock();
            continue;
        }
        pkg_t thispkg = qpkgRecv.front();
        qpkgRecv.pop();
        pkglk_recv.unlock();
        cout << "\n&&& [Module DPKG] DEAL:" << endl
             << "Operator:" << thispkg.head.wopr << endl
             << "Data:" << thispkg.jsdata << endl;
        mPkgfunc[thispkg.head.wopr](thispkg);
    }
}

//账户相关请求
int dpkg_login_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;
    reader.parse(pkg.jsdata, req);
    int ret = srv_checkpwd(req);
    res["un"] = req["un"].asString();
    res["res"] = ret;
    
    if (ret == 0) {
        UserMap[res["un"].asString()] = UserMap_T[pkg];
    } else {
        res["fd"] = UserMap_T[pkg].getfd();
    }

    UserMap_T.erase(pkg);
    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_LOGIN_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_register_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_register(req);
    res["un"] = req["un"].asString();
    res["res"] = ret;
    res["fd"] = UserMap_T[pkg].getfd();

    UserMap_T.erase(pkg);
    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_REG_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_resetpwd_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_resetpwd(req);
    res["un"] = req["un"].asString();
    res["res"] = ret;
    res["fd"] = UserMap_T[pkg].getfd();

    UserMap_T.erase(pkg);
    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_RESETPWD_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;  
}

//好友相关请求

int dpkg_addfriend_application(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    if (req["agr"].asInt() == 0) {
        res["res"] = srv_addfriend(req);
    } else {
        res["res"] = 1;
    }
    res["un"] = req["un"].asString();
    res["afwho"] = req["afwho"].asString();

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_ADDFRI_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_addfriend_request(pkg_t pkg) {
    pkglk_send.lock();
    qpkgSend.push(pkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_delfriend_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_delfriend(req);
    res["un"] = req["un"].asString();
    res["dfwho"] = req["dfwho"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_DELFRI_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_refreshfl_request(pkg_t pkg) {
    Json::Value req, res, root, friendlist, t;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    string fl;
    int ret = srv_getfl(req, fl);
    res["un"] = req["un"].asString();
    res["res"] = ret;

    reader.parse(fl, root);
    for (auto i = 0u; i < root["fl"].size(); i++) {
        t["fun"] = root["fl"][i].asString();
        t["fst"] = srv_getfst(req["un"].asString(), root["fl"][i].asString());
        friendlist.append(t);
    }
    res["fl"] = friendlist;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_REFRESH_FL;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;    
}

int dpkg_refreshgl_request(pkg_t pkg) {
    Json::Value req, res, root, grouplist, t;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    string fl;
    int ret = srv_getfl(req, fl);
    res["un"] = req["un"].asString();
    res["res"] = ret;

    reader.parse(fl, root);
    for (auto i = 0u; i < root["gl"].size(); i++) {
        t["gn"] = root["gl"][i].asString();
        t["gpw"] = srv_getgst(req["un"].asString(), root["gl"][i].asString());
        grouplist.append(t);
    }
    res["gl"] = grouplist;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_REFRESH_GL;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_mute_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_mute_enable(req);
    res["un"] = req["un"].asString();
    res["dfwho"] = req["mtwho"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_MUTE_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_dismute_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_mute_disable(req);
    res["un"] = req["un"].asString();
    res["dfwho"] = req["mtwho"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_DISMUTE_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

//群相关请求
int dpkg_refreshgm_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    string gml;
    int ret = srv_getgm(req, gml);
    reader.parse(gml, res);
    res["un"] = req["un"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_REFRESH_GM;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;    
}

int dpkg_creategroup_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_addgroup(req);
    res["un"] = req["un"].asString();
    res["gn"] = req["gn"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_CRGRP_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_dismissgroup_request(pkg_t pkg) {
    Json::Value req, res, gml;
    Json::Reader reader;
    Json::FastWriter writer;
    string st;

    reader.parse(pkg.jsdata, req);
    res["gn"] = req["gn"].asString();
    srv_getgm(req, st);
    reader.parse(st, gml);
    if (srv_delgroup(req) == 1)
        return 1;

    vector<string> gms;
    Json::Value t;
    t["gn"] = res["gn"].asString();
    for (auto i = 0u; i < gml["gml"].size(); i++) {
        res["un"] = gml["gml"][i].asString();

        t["un"] = res["un"].asString();
        srv_delfromgroup(t);
        pkg_t recvpkg;
        recvpkg.jsdata = writer.write(res);
        recvpkg.head.wopr = PT_DISGRP_NOT;
        recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

        pkglk_send.lock();
        qpkgSend.push(recvpkg);
        pcv_sendchanged.notify_one();
        pkglk_send.unlock();
    }
    return 0;
}

int dpkg_exitgroup_request(pkg_t pkg) {
    Json::Value req, res, r2;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_delfromgroup(req);
    res["egwho"] = req["egwho"].asString();
    res["gn"] = req["gn"].asString();
    r2["un"] = req["egwho"].asString();
    r2["gn"] = req["gn"].asString();

    pkg_t recvpkg, pcpkg;
    recvpkg.jsdata = writer.write(res);
    pcpkg.jsdata = writer.write(r2);
    recvpkg.head.wopr = PT_EXGRP_NOT;
    pcpkg.head.wopr = PT_EXGRP_PCNOT;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();
    pcpkg.head.datasize = sizeof(pkg_head_t) + pcpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    qpkgSend.push(pcpkg);
    pcv_sendchanged.notify_all();
    pkglk_send.unlock();
    return 0;
}

int dpkg_entergroup_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_addtogroup(req);
    res["un"] = req["un"].asString();
    res["gn"] = req["gn"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_ENGRP_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}

int dpkg_scmgr_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    
    if (req["typ"].asInt() == 0) {
        srv_setmgr(req["gn"].asString(), res["mgwho"].asString());
    } else {
        srv_canmgr(req["gn"].asString(), res["mgwho"].asString());
    }

    res["gn"] = req["gn"].asString();
    res["mgwho"] = req["mgwho"].asString();
    res["typ"] = req["typ"].asInt();

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_MGRC_NOT;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    pkglk_send.lock();
    qpkgSend.push(recvpkg);
    pcv_sendchanged.notify_one();
    pkglk_send.unlock();
    return 0;
}