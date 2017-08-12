#include<package.h>

void dpkg_init();
void dpkg_distribute();
//账户相关
int dpkg_login_request(pkg_t pkg);
int dpkg_register_request(pkg_t pkg);
int dpkg_resetpwd_request(pkg_t pkg);

//朋友相关
int dpkg_addfriend_request(pkg_t pkg);
int dpkg_delfriend_request(pkg_t pkg);
int dpkg_refreshfl_request(pkg_t pkg);
int dpkg_refreshgl_request(pkg_t pkg);
int dpkg_addfriend_application(pkg_t pkg);
int dpkg_mute_request(pkg_t pkg);
int dpkg_dismute_request(pkg_t pkg);

//群相关
int dpkg_refreshgm_request(pkg_t pkg);
int dpkg_creategroup_request(pkg_t pkg);
int dpkg_dismissgroup_request(pkg_t pkg);
int dpkg_exitgroup_request(pkg_t pkg);
int dpkg_entergroup_request(pkg_t pkg);
int dpkg_scmgr_request(pkg_t pkg);