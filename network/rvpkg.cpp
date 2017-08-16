#include<portal/network.h>
#include<portal/crypt.h>
#include<json_assist.h>
#include<watchdog.h>
#include<map>
#include<thread>
#include<rvpkg.h>
#include<iostream>
#include<condition_variable>
#include<algorithm>
using namespace std;
using namespace libportal;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
const string SERVER_SELFIP = "127.0.0.1";
const int SERVER_PORT = 6542;

map<string, TCPClient> UserMap;
map<pkg_t, TCPClient> UserMap_T;
extern condition_variable pcv_recvchanged;
extern vector<string> OnlineList;
MultiplexEpoll rv_epoll;

void rvpkg_init() {
    cout << "\n[Module RVPKG] Loaded!\n";
    thread thread_epl(rvpkg_distribute),
           thread_listen(rvpkg_listen);
    thread_epl.join();
    thread_listen.join();
}

void rvpkg_listen() {
    TCPSocket sock(SERVER_SELFIP, SERVER_PORT);
    sock.Listen();
    while (WATCHDOG_RVPKG) {
        TCPClient *clt = new TCPClient;
        *clt = sock.Accept();
        cout << "\n*** [Module RVPKG] ACCEPT:" << endl
             << "A Connetion Accepted." << endl;   
        rv_epoll.Add(*clt, EPOLLIN);
    }
}

void rvpkg_distribute() {
    MultiplexEpollEvent evts[100];
    while (WATCHDOG_RVPKG) {
        cout << "\n~~~ [Module RVPKG] WAIT:" << endl
            << "Epoll is waiting for connections." << endl;   
        int limit = rv_epoll.Wait(evts, 100);
        for (int i = 0; i < limit; i++) {
            TCPClient *clt = evts[i].GetClient();

            pkg_t thispkg;
            int ret = clt->Read((char *)&thispkg.head, sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                cout << "\n!!! [Module RVPKG] CLOSE:" << endl
                     << "A Connetion Closed." << endl;
                string b;
                for (auto i : UserMap) {
                    if (i.second.getfd() == clt->getfd()) {
                        b = i.first;
                    }
                }
                auto i = find(OnlineList.begin(), OnlineList.end(), b);
                if (i != OnlineList.end())
                    OnlineList.erase(i);
                clt->Close();
                //delete clt;
                continue;
            }
            ret = clt->Read(thispkg.jsdata, thispkg.head.datasize - sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                cout << "\n!!! [Module RVPKG] CLOSE:" << endl
                     << "A Connetion Closed." << endl;
                string b;
                for (auto i : UserMap) {
                    if (i.second.getfd() == clt->getfd()) {
                        b = i.first;
                    }
                }
                auto i = find(OnlineList.begin(), OnlineList.end(), b);
                if (i != OnlineList.end())
                    OnlineList.erase(i);      
                clt->Close();
                //delete clt;
                continue;
            }
            if (thispkg.head.wopr == PT_LOGIN_REQ || thispkg.head.wopr == PT_REG_REQ ||
                thispkg.head.wopr == PT_RESETPWD_REQ) {
                UserMap_T[thispkg] = *clt;
            }
            cout << "\n>>> [Module RVPKG] RECV:" << endl
                 << "Operator:" << thispkg.head.wopr
                 << "Data:" << thispkg.jsdata << endl;
            pkglk_recv.lock();
            qpkgRecv.push(thispkg);
            pcv_recvchanged.notify_all();
            pkglk_recv.unlock();
        }
    }
}
