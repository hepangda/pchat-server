#include<string>
#include<mysql++.h>
#include<json/json.h>
#include"test.h"

using namespace std;
using namespace mysqlpp;

mysqlpp::Connection dbconn(false);
char cache[10000];
int database_init() {
    int ret = dbconn.connect(CONNECT_STR);
    if (ret != 0) {
        //excited!
    }
}

int srv_register(Json::Value msg) {
    Query query = dbconn.query();
    static const char sqlhead[] = "insert into users(un,pwd,qst,ans,fl,gl,mute) value(";
    static const char sqltail[] = ",\"\",\"\",\"\");";

    sprintf(cache, "%s\"%s\",\"%s\",%d,\"%s\"%s", sqlhead, msg["un"].asCString(),
                    msg["pwd"].asCString(), msg["qst"].asInt(),
                    msg["ans"].asCString(), sqltail);
                    string nd = '\"' + msg["un"].asString() + "\",";
    query << cache;
    return query.exec();
}

int srv_checkpwd(Json::Value msg) {
    Query query = dbconn.query();
    static const char sqlhead[] = "select pwd from users where un=";
    static const char sqltail[] = ";";

    sprintf(cache, "%s\"%s\" and pwd=\"%s\"%s", sqlhead, msg["un"].asCString(),
    						msg["pwd"].asCString(), sqltail);
    query << cache;
    StoreQueryResult res = query.store();
    return res.size() == 1;
}

int srv_
/*
#include<json/json.h>
#include<iostream>
#include<string>
using namespace std;
Json::FastWriter wri;

int main() {
    Json::Value val;
    val["un"] = "pangda";
    val["pwd"] = "zcvjkapdo1278g123nz09131";
    val["type"] = true;
    string st = wri.write(val);
    cout << st;// << endl;
    return 0;
}
*/