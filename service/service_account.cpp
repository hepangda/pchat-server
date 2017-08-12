#include<service.h>

using namespace mysqlpp;
using namespace std;

extern mysqlpp::Connection dbconn;

//注册账户
//如果注册失败，SQL语句会执行失败，不必自己判断
//@param msg 请直接传收到包的json
//@return 0代表注册成功 1代表注册失败
int srv_register(Json::Value msg) {
    Query query = dbconn.query();
    char cache[50];
    sprintf(cache, "%d", msg["qst"].asInt());

    string sql = "insert into users(un,pwd,qst,ans,fl,gl,mute) value(\"" + 
                  msg["un"].asString() + "\",\"" + msg["pwd"].asString() +
                  "\"," + cache + ",\"" + msg["ans"].asString() + 
                  "\",\"{\\\"fl\\\":[]}\",\"{\\\"gl\\\":[]}\",\"{\\\"mute\\\":[]}\");";

    query << sql;
    return !query.exec();
}

int srv_resetpwd(Json::Value msg) {
    Query query = dbconn.query();
    char cache[50];
    sprintf(cache, "%d", msg["qst"].asInt());
    string sql = "select * from users where un=\"" + msg["un"].asString() +
                 "\" and qst = \"" + cache + "\" and ans=\"" + msg["ans"].asString() + "\";";
    
    query << sql;
    StoreQueryResult res = query.store();
    if (res.size() != 1)
        return 1;
    sql = "update users set pwd=\"" + msg["pwd"].asString() + "\" where un=\"" +
          msg["un"].asString() + "\";";
    query << sql;
    return !query.exec();
}

//验证密码是否正确
//@param msg 收到的包的Json
//@return 0代表用户名密码匹配，1代表用户名密码不匹配
int srv_checkpwd(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "select pwd from users where un=\"" + msg["un"].asString() +
                 "\" and pwd =\"" + msg["pwd"].asString() + "\";";

    query << sql;
    StoreQueryResult res = query.store();
    return res.size() != 1;
}

//验证是否有此用户
//@param un 要验证的用户名
//@return 0代表有此用户，1代表无此用户
int srv_checkuser(string un) {
    Query query = dbconn.query();
    string sql = "select un from users where un=\"" + un + "\";";
    query << sql;

    StoreQueryResult res = query.store();
    return res.size() != 1;
}