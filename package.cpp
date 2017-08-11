#include<service.h>
#include<queue>
#include<mutex>
#include<map>
#include<functional>
#include<package.h>


using namespace std;
int dpkg_login_request(pkg_t pkg);
queue<pkg_t> qPackage;
queue<pkg_t> qSend;
mutex lkPackage;
map<uint16_t, function<int(pkg_t)> > mPkgfunc;
extern bool WATCHDOG_DPKG;

void dpkg_init() {
    // mPkgfunc[PT_LOGIN_REQ] = dpkg_login_request;
    // mPkgfunc[PT_REG_REQ] = dpkg_register_request;
    // mPkgfunc[PT_RESETPWD_REQ] = ;
    // mPkgfunc[PT_ADDFRI_REQ] = dpkg_addfriend_request;
    // mPkgfunc[PT_DELFRI_REQ] = ;
    // mPkgfunc[PT_ADDFRI_APP] = ;
    // mPkgfunc[PT_ONLINE] = ;
    // mPkgfunc[PT_OFFLINE] = ;
    // mPkgfunc[PT_REFRESH_FL] = ;
    // mPkgfunc[PT_REFRESH_GL] = ;
    // mPkgfunc[PT_REFRESH_GM] = ;
    // mPkgfunc[PT_MSG_GRP] = ;
    // mPkgfunc[PT_MSG_FRI] = ;
    // mPkgfunc[PT_PKEY_REQ] = ;
    // mPkgfunc[PT_PKEY_RES] = ;
    // mPkgfunc[PT_MUTE_REQ] = ;
    // mPkgfunc[PT_SCMGR_REQ] = ;
    // mPkgfunc[PT_EXGRP_REQ] = ;
}

void dpkg_distribute() {
    while (WATCHDOG_DPKG) {
        lkPackage.lock();
        pkg_t thispkg = qPackage.front();
        qPackage.pop();
        lkPackage.unlock();

        mPkgfunc[thispkg.head.wopr](thispkg);
    }
}

int dpkg_login_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_checkpwd(req);
    res["un"] = req["un"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_LOGIN_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    qSend.push(recvpkg);
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

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_REG_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    qSend.push(recvpkg);
    return 0;
}

int dpkg_addfriend_request(pkg_t pkg) {
    Json::Value req, res;
    Json::Reader reader;
    Json::FastWriter writer;

    reader.parse(pkg.jsdata, req);
    int ret = srv_addfriend(req);
    res["un"] = req["un"].asString();
    res["afwho"] = req["afwho"].asString();
    res["res"] = ret;

    pkg_t recvpkg;
    recvpkg.jsdata = writer.write(res);
    recvpkg.head.wopr = PT_ADDFRI_RES;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    qSend.push(recvpkg);
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

    qSend.push(recvpkg);
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

    qSend.push(recvpkg);
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
    cout << recvpkg.jsdata << endl;
    recvpkg.head.wopr = PT_REFRESH_GL;
    recvpkg.head.datasize = sizeof(pkg_head_t) + recvpkg.jsdata.size();

    qSend.push(recvpkg);
    return 0;
}