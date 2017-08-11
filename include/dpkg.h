#include<package.h>

void dpkg_init();
void dpkg_distribute();
int dpkg_login_request(pkg_t pkg);
int dpkg_register_request(pkg_t pkg);
int dpkg_addfriend_request(pkg_t pkg);
int dpkg_delfriend_request(pkg_t pkg);
int dpkg_refreshfl_request(pkg_t pkg);
int dpkg_refreshgl_request(pkg_t pkg);
int dpkg_refreshgm_request(pkg_t pkg);
