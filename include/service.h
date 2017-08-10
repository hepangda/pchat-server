#include<json_assist.h>
#include<mysql++.h>

int database_init();
int srv_register(Json::Value msg);
int srv_checkpwd(Json::Value msg);
int srv_checkuser(std::string un);
int srv_addfriend(Json::Value msg);
int srv_delfriend(Json::Value msg);
int srv_getfl(Json::Value msg, std::string &store);