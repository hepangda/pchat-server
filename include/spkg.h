#include<package.h>

void spkg_init();
void spkg_distribute();
int spkg_to_singleuser(pkg_t pkg);
int spkg_to_group(pkg_t pkg);
int spkg_to_eachother(pkg_t pkg);
int spkg_to_afwho(pkg_t pkg);
int spkg_special_addfri(pkg_t pkg);
int spkg_special_login(pkg_t pkg);
int spkg_special_notlogon(pkg_t pkg);