#include<queue>
#include<mutex>
#include<map>
#include<functional>
#include<package.h>

using namespace std;
int dpkg_login_request(pkg_t pkg);
queue<pkg_t> qPackage;
mutex lkPackage;
map<uint16_t, function<int(pkg_t)> > mPkgfunc;
extern bool WATCHDOG_DPKG;

void dpkg_init() {
    mPkgfunc[PT_LOGIN_REQ] = dpkg_login_request;
    // mPkgfunc[PT_REG_REQ] = ;
    // mPkgfunc[PT_RESETPWD_REQ] = ;
    // mPkgfunc[PT_ADDFRI_REQ] = ;
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
    return 0;
}