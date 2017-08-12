#include<portal/network.h>
#include<portal/crypt.h>
#include<json_assist.h>
#include<watchdog.h>
#include<map>
#include<thread>
#include<rvpkg.h>
using namespace std;
using namespace libportal;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
const string SERVER_SELFIP = "127.0.0.1";
const int SERVER_PORT = 14001;

map<string, TCPClient> UserMap;
map<pkg_t, TCPClient> UserMap_T;

MultiplexEpoll rv_epoll;

void rvpkg_init() {
    TCPSocket sock(SERVER_SELFIP, SERVER_PORT);
    sock.Listen();

    thread thread_epl(rvpkg_distribute);
    while (WATCHDOG_RVPKG) {
        TCPClient *clt = new TCPClient;
        *clt = sock.Accept();
        rv_epoll.Add(*clt, EPOLLIN | EPOLLET);
    }
}

void rvpkg_distribute() {
    MultiplexEpollEvent evts[100];
    while (WATCHDOG_RVPKG) {
        int limit = rv_epoll.Wait(evts, 100);
        for (int i = 0; i < limit; i++) {
            TCPClient *clt = evts[i].GetClient();

            pkg_t thispkg;
            int ret = clt->Read((char *)&thispkg.head, sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                clt->Close();
                delete clt;
                continue;
            }
            ret = clt->Read(thispkg.jsdata, thispkg.head.datasize);
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                clt->Close();
                delete clt;
                continue;
            }
            if (thispkg.head.wopr == PT_LOGIN_REQ) {
                UserMap_T[thispkg] = *clt;
            }
            pkglk_recv.lock();
            qpkgRecv.push(thispkg);
            pkglk_recv.unlock();
        }
    }
}