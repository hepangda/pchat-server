#include<package.h>
#include<mutex>
#include<queue>

std::queue<pkg_t> qpkgRecv;
std::queue<pkg_t> qpkgSend;
std::mutex pkglk_recv;
std::mutex pkglk_send;
std::vector<std::string> OnlineList;