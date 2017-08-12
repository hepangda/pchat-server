#include<json_assist.h>
#include<mysql++.h>

//service_database
int database_init();

//service_account
int srv_register(Json::Value msg);
int srv_checkpwd(Json::Value msg);
int srv_checkuser(std::string un);
int srv_resetpwd(Json::Value msg);

//service_friend
int srv_addfriend(Json::Value msg);
int srv_delfriend(Json::Value msg);
int srv_getfl(Json::Value msg, std::string &store);
int srv_getgl(Json::Value msg, std::string &store);
int srv_getfst(std::string un, std::string who);
int srv_mute_enable(Json::Value msg);
int srv_mute_disable(Json::Value msg);

//service_group
int srv_addgroup(Json::Value msg);
int srv_delgroup(Json::Value msg);
int srv_getgst(std::string gn, std::string un);
int srv_addtogroup(Json::Value msg);
int srv_delfromgroup(Json::Value msg);
int srv_getgm(Json::Value msg, std::string &store);
int srv_setmgr(std::string gn, std::string afwho);
int srv_canmgr(std::string gn, std::string dfwho);
std::string srv_getcrtname(std::string gn);


//service_record
int srv_delcr_private(std::string un1, std::string un2);
int srv_delcr_group(std::string gn);
int srv_cr_private(std::string from, std::string to, std::string content);
int srv_cr_group(std::string gn, std::string un, std::string content);
