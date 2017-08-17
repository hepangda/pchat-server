#include<condition_variable>
#include<portal/network.h>
#include<portal/crypt.h>
#include<json_assist.h>
#include<watchdog.h>
#include<algorithm>
#include<iostream>
#include<rvpkg.h>
#include<thread>
#include<map>
#define PKGRECV_PUSH(pkg) pkglk_recv.lock(); qpkgRecv.push(pkg); pcv_recvchanged.notify_all(); pkglk_recv.unlock()

using namespace std;
using namespace libportal;

const int SERVER_PORT = 6541;
const string SERVER_SELFIP = "127.0.0.1";
map<string, TCPClient> UserMap;
map<pkg_t, TCPClient> UserMap_T;
MultiplexEpoll rv_epoll;

EXTERN_PKG_QM;
EXTERN_WATCHDOGS;
extern condition_variable pcv_recvchanged;
extern vector<string> OnlineList;

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
    WHILE_WATCHDOG(RVPKG) {
        TCPClient *clt = new TCPClient;
        *clt = sock.Accept();

        cout << "*** \033[31m ACCEPT:\nA Connection Accepted\033[0m\n";
        rv_epoll.Add(*clt, EPOLLIN);
    }
}

void rvpkg_distribute() {
    MultiplexEpollEvent evts[100];
    WHILE_WATCHDOG(RVPKG) {
        cout << "~~~ \033[31m WAIT:\nEpoll is waiting for connections.\033[0m\n";   
        int limit = rv_epoll.Wait(evts, 100);
        for (int i = 0; i < limit; i++) {
            TCPClient *clt = evts[i].GetClient();

            pkg_t thispkg;
            int ret = clt->Read((char *)&thispkg.head, sizeof(pkg_head_t));
            if (ret == 0) {
                rv_epoll.Delete(*clt);
                cout << "!!! \033[31m CLOSE:\nA Connetion Closed.\033[0m\n" << endl;
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
                cout << "!!! \033[31m CLOSE:\nA Connetion Closed.\033[0m\n" << endl;

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
            cout << ">>> \033[31m RECV:\nOperator:" << thispkg.head.wopr
                 << "\nData:" << thispkg.jsdata << "\033[0m\n";
            PKGRECV_PUSH(thispkg);
        }
    }
}
