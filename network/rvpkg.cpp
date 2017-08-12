#include<portal/network.h>
#include<portal/crypt.h>
#include<json_assist.h>
#include<watchdog.h>
#include<map>
#include<thread>
#include<rvpkg.h>
#include<iostream>
#include<condition_variable>
using namespace std;
using namespace libportal;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
const string SERVER_SELFIP = "127.0.0.1";
const int SERVER_PORT = 14001;

map<string, TCPClient> UserMap;
map<pkg_t, TCPClient> UserMap_T;
extern condition_variable pcv_recvchanged;

MultiplexEpoll rv_epoll;

void rvpkg_init() {
    cout << "[Module RVPKG] Loaded!" << endl;
    thread thread_epl(rvpkg_distribute);
    thread_epl.detach();
    thread thread_listen(rvpkg_listen);
    thread_listen.detach();
}

void rvpkg_listen() {
    TCPSocket sock(SERVER_SELFIP, SERVER_PORT);
    sock.Listen();
    while (WATCHDOG_RVPKG) {
        TCPClient *clt = new TCPClient;
        *clt = sock.Accept();
        cout << "New Connection Accepted.FD:" << clt->getfd() << endl;
        rv_epoll.Add(*clt, EPOLLIN);
    }
}

void rvpkg_distribute() {
    MultiplexEpollEvent evts[100];
    while (WATCHDOG_RVPKG) {
        cout << "EPOLL WAITING.." << endl;
        int limit = rv_epoll.Wait(evts, 100);
        for (int i = 0; i < limit; i++) {
            TCPClient *clt = evts[i].GetClient();

            pkg_t thispkg;
            int ret = clt->Read((char *)&thispkg.head, sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                cout << "A Connetion Close By Epoll Read 1" << endl;
                clt->Close();
                delete clt;
                continue;
            }
            ret = clt->Read(thispkg.jsdata, thispkg.head.datasize - sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                cout << "A Connetion Close By Epoll Read 2" << endl;                
                clt->Close();
                delete clt;
                continue;
            }
            if (thispkg.head.wopr == PT_LOGIN_REQ) {
                UserMap_T[thispkg] = *clt;
            }
            cout << "Recv A pack =#" << thispkg.jsdata << "#" << endl;
            pkglk_recv.lock();
            qpkgRecv.push(thispkg);
            pcv_recvchanged.notify_all();
            pkglk_recv.unlock();
        }
    }
}