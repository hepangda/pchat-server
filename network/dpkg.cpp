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
#include<algorithm>
using namespace std;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
#define DPKG_WAITCV unique_lock<mutex> dpkg__ulrc(dpkg__mtxrc);  pcv_recvchanged.wait(dpkg__ulrc)
#define DPKG_SENDPKG(pkg) pkglk_send.lock(); qpkgSend.push(pkg); \
                     pcv_sendchanged.notify_all(); pkglk_send.unlock()
#define DPKG_BIND(msg, func) dpkg__func[msg] = func
#define DPKG_TOFUNCTION(pkg) dpkg__func[pkg.head.wopr](pkg)
#define JSON_MAKEOTH(what, item, value) what[item]=value
#define JSON_MAKERES(item, value) res[item]=value
#define JSON_REQUEST (req)
#define DPKG_GETUTFD(pkg) (UserMap_T[pkg].getfd())
#define DPKG_ERASEUT(pkg) UserMap_T.erase(pkg)
#define OLST_BOOK(un, pkg) OnlineList.push_back(un); UserMap[un] = UserMap_T[pkg]
#define DPKG_WRITEPKG(pkg, opr) pkg.jsdata = writer.write(res); pkg.head.wopr = opr; \
                                pkg.head.datasize = sizeof(pkg_head_t) + pkg.jsdata.size();
#define JSON_CPREQSTR(item) res[item]=req[item].asString()
#define JSON_CPREQINT(item) res[item]=req[item].asInt()
#define JSON_PARSE(str, jsonv) reader.parse(str, jsonv)
#define JSON_ARRITER(jv, name) for (auto i = 0u; i < (jv[name].size()); i++)
#define JSON_ARRSIZE(jv, name) (jv[name].size())
#define JSON_ARRITER_ITEMSTR(jv, item) (jv[item][i].asString())

#define PKGRECV_LOCK pkglk_recv.lock()
#define PKGRECV_UNLOCK pkglk_recv.unlock()
#define PKGRECV_EMPTY (qpkgRecv.empty())
#define PKGRECV_POPASSIGN qpkgRecv.front(); qpkgRecv.pop()

#define JSON_PARSEONLY Json::Value root; Json::Reader reader; reader.parse(pkg.jsdata, root)
#define JSON_PARSEW Json::Value req, res; Json::Reader reader; Json::FastWriter writer; \
                    reader.parse(pkg.jsdata, req)
#define JSON_GETSTRING(item) (root[item].asString())
#define JSON_GETINT(item) (root[item].asInt())
#define JSON_GETREQSTR(item) (req[item].asString())
#define JSON_GETREQINT(item) (req[item].asInt())
#define OLST_FIND(un) (find(OnlineList.begin(), OnlineList.end(), un))
#define OLST_NOTFOUND (OnlineList.end())

static map<uint16_t, function<int(pkg_t)> > dpkg__func;
extern map<string, libportal::TCPClient> UserMap;
extern map<pkg_t, libportal::TCPClient> UserMap_T;
extern condition_variable pcv_sendchanged;
extern vector<string> OnlineList;
mutex dpkg__mtxrc;
condition_variable pcv_recvchanged;


void dpkg_init() {
    cout << "\033[32m[Module DPKG] Loaded!\033[0m\n";

    DPKG_BIND(PT_LOGIN_REQ, dpkg_login_request);
    DPKG_BIND(PT_REG_REQ, dpkg_register_request);
    DPKG_BIND(PT_RESETPWD_REQ, dpkg_resetpwd_request);

    DPKG_BIND(PT_ADDFRI_REQ, dpkg_addfriend_request);
    DPKG_BIND(PT_DELFRI_REQ, dpkg_delfriend_request);
    DPKG_BIND(PT_ADDFRI_APP, dpkg_addfriend_application);
    DPKG_BIND(PT_REFRESH_FL, dpkg_refreshfl_request);
    DPKG_BIND(PT_REFRESH_GL, dpkg_refreshgl_request);
    DPKG_BIND(PT_MUTE_REQ, dpkg_mute_request);
    DPKG_BIND(PT_DISMUTE_REQ, dpkg_dismute_request);
    DPKG_BIND(PT_MSG_FRI, dpkg_msg_friend);

    DPKG_BIND(PT_REFRESH_GM, dpkg_refreshgm_request);
    DPKG_BIND(PT_CRGRP_REQ, dpkg_creategroup_request);
    DPKG_BIND(PT_DISGRP_REQ, dpkg_dismissgroup_request);
    DPKG_BIND(PT_EXGRP_REQ, dpkg_exitgroup_request);
    DPKG_BIND(PT_ENGRP_REQ, dpkg_entergroup_request);
    DPKG_BIND(PT_SCMGR_REQ, dpkg_scmgr_request);
    DPKG_BIND(PT_MSG_GRP, dpkg_msg_grp);
    DPKG_BIND(PT_FCH_REQ, dpkg_fch);
    DPKG_BIND(PT_FETCHCR_REQ, dpkg_fetchcr);

    thread dpkg_threads[3];
    for (int i = 0; i < 3; i++) {
        dpkg_threads[i] = thread(dpkg_distribute);
    }

    for (int i = 0; i < 3; i++) {
        dpkg_threads[i].join();
    }
}


void dpkg_distribute() {
    WHILE_WATCHDOG(DPKG) {
        DPKG_WAITCV;

        PKGRECV_LOCK;
        if (PKGRECV_EMPTY) {
            PKGRECV_UNLOCK;
            continue;
        }

        pkg_t thispkg = PKGRECV_POPASSIGN;
        PKGRECV_UNLOCK;

        cout << "&&& \033[32m DEAL:\nOperator:" << thispkg.head.wopr
             << "\nData:" << thispkg.jsdata << "\033[0m\n";
        DPKG_TOFUNCTION(thispkg);
    }
}

//聊天相关请求
int dpkg_msg_grp(pkg_t pkg) {
    JSON_PARSEONLY;
    srv_cr_group(JSON_GETSTRING("gn"), JSON_GETSTRING("sd"), JSON_GETSTRING("msg"));
    DPKG_SENDPKG(pkg);
    return 0;
}

int dpkg_msg_friend(pkg_t pkg) {
    JSON_PARSEONLY;

    int online = (OLST_FIND(JSON_GETSTRING("rv")) != OLST_NOTFOUND);
    if (srv_getfst(JSON_GETSTRING("rv"), JSON_GETSTRING("sd")) == 2)
        return 0;
    srv_cr_private(JSON_GETSTRING("sd"), JSON_GETSTRING("rv"), JSON_GETSTRING("msg"), online);

    DPKG_SENDPKG(pkg);
    return 0;
}

//账户相关请求
int dpkg_login_request(pkg_t pkg) {
    JSON_PARSEW;

    JSON_CPREQSTR("un");

    int ret = srv_checkpwd(JSON_REQUEST);
    JSON_MAKERES("res", ret);

    if (OLST_FIND(JSON_GETREQSTR("un")) != OLST_NOTFOUND) {
        JSON_MAKERES("res", ret = 1);
    }

    if (ret != 0) {
        JSON_MAKERES("fd", DPKG_GETUTFD(pkg));
    } else {
        OLST_BOOK(JSON_GETREQSTR("un"), pkg);
    }

    DPKG_ERASEUT(pkg);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_LOGIN_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_register_request(pkg_t pkg) {
    JSON_PARSEW;

    JSON_CPREQSTR("un");
    JSON_MAKERES("res", srv_register(JSON_REQUEST));
    JSON_MAKERES("fd", DPKG_GETUTFD(pkg));

    DPKG_ERASEUT(pkg);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_REG_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_resetpwd_request(pkg_t pkg) {
    JSON_PARSEW;

    JSON_CPREQSTR("un");
    JSON_MAKERES("res", srv_resetpwd(JSON_REQUEST));
    JSON_MAKERES("fd", DPKG_GETUTFD(pkg));

    DPKG_ERASEUT(pkg);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_RESETPWD_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;  
}

//好友相关请求

int dpkg_addfriend_application(pkg_t pkg) {
    JSON_PARSEW;
    JSON_CPREQSTR("un");
    JSON_CPREQSTR("afwho");
    JSON_MAKERES("agr", ((JSON_GETREQINT("agr") == 0) 
                        ? srv_addfriend(JSON_REQUEST) : 1));

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_ADDFRI_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_addfriend_request(pkg_t pkg) {
    DPKG_SENDPKG(pkg);
    return 0;
}

int dpkg_delfriend_request(pkg_t pkg) {
    JSON_PARSEW;
    JSON_CPREQSTR("un");
    JSON_CPREQSTR("dfwho");
    JSON_MAKERES("ret", srv_delfriend(JSON_REQUEST));

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_DELFRI_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_refreshfl_request(pkg_t pkg) {
    JSON_PARSEW;
    Json::Value root, friendlist, t;

    string fl;
    JSON_CPREQSTR("un");
    JSON_MAKERES("res", srv_getfl(JSON_REQUEST, fl));
    JSON_PARSE(fl, root);

    reader.parse(fl, root);
    JSON_ARRITER(root, "fl") {
        JSON_MAKEOTH(t, "fun", JSON_ARRITER_ITEMSTR(root, "fl"));
        JSON_MAKEOTH(t, "fst", srv_getfst(JSON_GETREQSTR("un"), JSON_ARRITER_ITEMSTR(root, "fl")));
        friendlist.append(t);
    }
    JSON_MAKERES("fl", friendlist);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_FL);
    DPKG_SENDPKG(recvpkg);
    return 0;    
}

int dpkg_refreshgl_request(pkg_t pkg) {
    JSON_PARSEW;
    Json::Value root, grouplist, t;

    string gl;
    JSON_CPREQSTR("un");
    JSON_MAKERES("res", srv_getgl(JSON_REQUEST, gl));
    JSON_PARSE(gl, root);

    JSON_ARRITER(root, "gl") {
        JSON_MAKEOTH(t, "gn", JSON_ARRITER_ITEMSTR(root, "gl"));
        JSON_MAKEOTH(t, "gpw", srv_getgst(JSON_GETREQSTR("un"), JSON_ARRITER_ITEMSTR(root, "gl")));
        grouplist.append(t);
    }
    JSON_MAKERES("gl", grouplist);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_GL);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_mute_request(pkg_t pkg) {
    JSON_PARSEW;
    JSON_CPREQSTR("un");
    JSON_CPREQSTR("mtwho");
    JSON_MAKERES("res", srv_mute_enable(JSON_REQUEST));

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_MUTE_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_dismute_request(pkg_t pkg) {
    JSON_PARSEW;
    JSON_CPREQSTR("un");
    JSON_CPREQSTR("mtwho");
    JSON_MAKERES("res", srv_mute_disable(JSON_REQUEST));

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_DISMUTE_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

//群相关请求
int dpkg_refreshgm_request(pkg_t pkg) {
    JSON_PARSEW;
    string gml;
    int ret = srv_getgm(req, gml);
    reader.parse(gml, res);

    JSON_CPREQSTR("un");
    JSON_CPREQSTR("gn");
    JSON_MAKERES("res", ret);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_GM);
    DPKG_SENDPKG(recvpkg);
    return 0;    
}

int dpkg_creategroup_request(pkg_t pkg) {
    JSON_PARSEW;

    JSON_CPREQSTR("un");
    JSON_CPREQSTR("gn");
    JSON_MAKERES("res", srv_addgroup(JSON_REQUEST));

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_CRGRP_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_dismissgroup_request(pkg_t pkg) {
    JSON_PARSEW;
    Json::Value gml, t;
    string st;

    JSON_CPREQSTR("gn");

    srv_getgm(JSON_REQUEST, st);
    JSON_PARSE(st, gml);
    if (srv_delgroup(JSON_REQUEST) == 1)
        return 1;

    JSON_MAKEOTH(t, "gn", JSON_GETREQSTR("gn"));
    JSON_ARRITER(gml, "gml") {
        JSON_MAKEOTH(t, "un", JSON_GETREQSTR("un"));
        JSON_MAKERES("un", JSON_ARRITER_ITEMSTR(gml, "gml"));

        srv_delfromgroup(t);
        pkg_t recvpkg;
        DPKG_WRITEPKG(recvpkg, PT_DISGRP_NOT);
        DPKG_SENDPKG(recvpkg);
    }
    return 0;
}

int dpkg_exitgroup_request(pkg_t pkg) {
    JSON_PARSEW;
    Json::Value r2;

    srv_delfromgroup(req);
    JSON_CPREQSTR("egwho");
    JSON_CPREQSTR("gn");
    JSON_MAKEOTH(r2, "un", JSON_GETREQSTR("egwho"));
    JSON_MAKEOTH(r2, "gn", JSON_GETREQSTR("gn"));

    pkg_t recvpkg, pcpkg;
    DPKG_WRITEPKG(recvpkg, PT_EXGRP_NOT);
    DPKG_WRITEPKG(pcpkg, PT_EXGRP_PCNOT);
    DPKG_SENDPKG(recvpkg);
    DPKG_SENDPKG(pcpkg);
    return 0;
}

int dpkg_entergroup_request(pkg_t pkg) {
    JSON_PARSEW;

    int ret = srv_addtogroup(JSON_REQUEST);
    JSON_CPREQSTR("un");
    JSON_CPREQSTR("gn");
    JSON_MAKERES("res", ret);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, ((ret != 0) ? PT_ENGRP_PCNOT : PT_ENGRP_RES));
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_scmgr_request(pkg_t pkg) {
    JSON_PARSEW;
    
    (JSON_GETREQINT("typ") == 0) ?
        srv_setmgr(JSON_GETREQSTR("gn"), JSON_GETREQSTR("mgwho")) :
        srv_canmgr(JSON_GETREQSTR("gn"), JSON_GETREQSTR("mgwho"));

    JSON_CPREQSTR("gn");
    JSON_CPREQSTR("mgwho");
    JSON_CPREQINT("typ");

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_MGRC_NOT);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_fetchcr(pkg_t pkg) {
    JSON_PARSEW;

    string store;
    (JSON_GETREQINT("typ") == 0) ?
        srv_fetch_privatecr(JSON_GETREQSTR("fun"), JSON_GETREQSTR("un"), store) :
        srv_fetch_grpcr(JSON_GETREQSTR("gn"), store);

    JSON_CPREQSTR("un");
    JSON_CPREQINT("typ");
    JSON_MAKERES("rc", store);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_FETCHCR_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}

int dpkg_fch(pkg_t pkg) {
    JSON_PARSEW;

    string store;
    srv_fch(JSON_GETREQSTR("un"), store);

    JSON_CPREQSTR("un");
    JSON_MAKERES("rc", store);

    pkg_t recvpkg;
    DPKG_WRITEPKG(recvpkg, PT_FCH_RES);
    DPKG_SENDPKG(recvpkg);
    return 0;
}