#include<service.h>

using namespace mysqlpp;
using namespace std;
static char cache[10000];//FIXME:TO ENSURE THREAD SAFE!!!!

extern mysqlpp::Connection dbconn;
//注册账户
//如果注册失败，SQL语句会执行失败，不必自己判断
//@param msg 请直接传收到包的json
//@return 0代表注册成功 1代表注册失败

int srv_register(Json::Value msg) {
    Query query = dbconn.query();
    static const char sqlhead[] = "insert into users(un,pwd,qst,ans,fl,gl,mute) value(";
    static const char sqltail[] = ",\"{\"fl\":[]}\",\"{\"gl\":[]}\",\"{\"mute\":[]    }\");";

    sprintf(cache, "%s\"%s\",\"%s\",%d,\"%s\"%s", sqlhead, msg["un"].asCString(),
                    msg["pwd"].asCString(), msg["qst"].asInt(),
                    msg["ans"].asCString(), sqltail);
                    string nd = '\"' + msg["un"].asString() + "\",";
    query << cache;
    return !query.exec();
}

//验证密码是否正确
//@param msg 收到的包的Json
//@return 0代表用户名密码匹配，1代表用户名密码不匹配
int srv_checkpwd(Json::Value msg) {
    Query query = dbconn.query();
    sprintf(cache, "select pwd from users where un=\"%s\" and pwd=\"%s\";", 
                            msg["un"].asCString(), msg["pwd"].asCString());
    query << cache;
    StoreQueryResult res = query.store();
    return res.size() != 1;
}

//验证是否有此用户
//@param un 要验证的用户名
//@return 0代表有此用户，1代表无此用户
int srv_checkuser(string un) {
    Query query = dbconn.query();
    sprintf(cache, "select un from users where un=%s;", un.c_str());
    query << cache;
    StoreQueryResult res = query.store();
    return res.size() != 1;
}