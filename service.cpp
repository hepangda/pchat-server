#include<json_assist.h>
#include<mysql++.h>

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
    static const char sqltail[] = ",\"{\"fl\":[]}\",\"{\"gl\":[]}\",\"{\"mute\":[]    }\");";

    sprintf(cache, "%s\"%s\",\"%s\",%d,\"%s\"%s", sqlhead, msg["un"].asCString(),
                    msg["pwd"].asCString(), msg["qst"].asInt(),
                    msg["ans"].asCString(), sqltail);
                    string nd = '\"' + msg["un"].asString() + "\",";
    query << cache;
    return !query.exec();
}

int srv_checkpwd(Json::Value msg) {
    Query query = dbconn.query();
    sprintf(cache, "select pwd from users where un=\"%s\" and pwd=\"%s\";", 
                            msg["un"].asCString(), msg["pwd"].asCString());
    query << cache;
    StoreQueryResult res = query.store();
    return res.size() != 1;
}

int srv_addfriend_atomic(string un, string afwho) {
    Query query = dbconn.query();
    sprintf(cache, "select fl from users where un=\"%s\";", un.c_str());
    cout << cache << endl;

    query << cache;

    StoreQueryResult res = query.store();

    Json::Value fl;
    j_reader.parse(res[0]["fl"].c_str(), fl);
    if (json_append(fl, "fl", afwho) == -1) {
        return -1;
    }
    string result = j_writer.write(fl);
    json_tosql(result);

    sprintf(cache, "update users set fl=\"%s\" where un=\"%s\";", result.c_str(), un.c_str());
	cout << cache << endl;
    query << cache;
    return !query.exec();
}
