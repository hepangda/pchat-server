#include<service.h>

using namespace mysqlpp;
using namespace std;
static char cache[10000];//FIXME:TO ENSURE THREAD SAFE!!!!
extern mysqlpp::Connection dbconn;


int srv_addgroup(Json::Value msg) {
    Query query = dbconn.query();
    static const char sqlhead[] = "insert into groups(gn,gml,gcreater,gmgr) value(";
    static const char sqltail[] = ", \"{\\\"gmgr\\\":[]}\");";

    sprintf(cache, "%s\"%s\",\"{\\\"gm\\\":[]}\",\"%s\"%s", sqlhead, msg["gn"].asCString(),
                    msg["un"].asCString(), sqltail);
    query << cache;
    return !query.exec();
}

int srv_delgroup(Json::Value msg) {
    int power = srv_getgst(msg["gn"].asString(), msg["un"].asString());
    if (power != 0)
        return 1;
    
    Query query = dbconn.query();
    sprintf(cache, "delete from groups where gn=%s", msg["gn"].asCString());
    query << cache;
    //TODO:delete all record
    return !query.exec();
}

//@return 2代表成员，1代表管理员，0代表群主
int srv_getgst(string gn, string un) {
    Query query = dbconn.query();
    sprintf(cache, "select gml from groups where gn=%s;", gn.c_str());
    query << cache;
    
    StoreQueryResult res = query.store();
    Json::Value gm;
    j_reader.parse(res[0]["gmgr"].c_str(), gm);
    int ret = json_findarray(gm, "gm", un.c_str());
    if (ret != -1)
        return 1;

    sprintf(cache, "select gcreater from groups where gn=%s;", gn.c_str());
    query << cache;
    res = query.store();

    if (gn == string(res[0]["gcreater"].c_str()))
        return 0;

    return 2;
}

int srv_addtogroup(Json::Value msg) {
    Query query = dbconn.query();
    sprintf(cache, "select gl from users where un=\"%s\";", msg["un"].asCString());
    query << cache;

    StoreQueryResult res = query.store();

    Json::Value gl;
    j_reader.parse(res[0]["gl"].c_str(), gl);
    if (json_append(gl, "gl", msg["gn"].asString()) == -1) {
        return 1;
    }
    string result = j_writer.write(gl);
    json_tosql(result);

    sprintf(cache, "update users set gl=\"%s\" where un=\"%s\";", 
                        result.c_str(), msg["un"].asCString());
    query << cache;
    if(!query.exec()) {
        return 1;
    }

    sprintf(cache, "select gml from groups where un=\"%s\";", msg["un"].asCString());
    query << cache;

    res = query.store();

    Json::Value gml;
    j_reader.parse(res[0]["gml"].c_str(), gml);
    if (json_append(gml, "gml", msg["un"].asString()) == -1) {
        return 1;
    }
    result = j_writer.write(gml);
    json_tosql(result);

    sprintf(cache, "update groups set gml=\"%s\" where gn=\"%s\";",
                         result.c_str(), msg["gn"].asCString());
    query << cache;
    return !query.exec();
}

int srv_delfromgroup(Json::Value msg) {
    Query query = dbconn.query();
    sprintf(cache, "select gl from users where un=\"%s\";", msg["un"].asCString());
    query << cache;

    StoreQueryResult res = query.store();

    Json::Value gl;
    j_reader.parse(res[0]["gl"].c_str(), gl);
    int idx = json_findarray(gl, "gl", msg["gn"].asString());
    if (idx == -1) {
        return 1;
    }
    Json::Value done;
    gl["gl"].removeIndex(idx, &done);
    string result = j_writer.write(gl);
    json_tosql(result);

    sprintf(cache, "update users set gl=\"%s\" where un=\"%s\";", 
                        result.c_str(), msg["un"].asCString());
    query << cache;
    if(!query.exec()) {
        return 1;
    }

    sprintf(cache, "select gml from groups where un=\"%s\";", msg["un"].asCString());
    query << cache;

    res = query.store();

    Json::Value gml;
    j_reader.parse(res[0]["gml"].c_str(), gml);
    idx = json_findarray(gml, "gml", msg["un"].asString());
    if (idx == -1) {
        return 1;
    }
    Json::Value done1;
    gml["gml"].removeIndex(idx, &done1);
    result = j_writer.write(gml);
    json_tosql(result);

    sprintf(cache, "update groups set gml=\"%s\" where gn=\"%s\";",
                         result.c_str(), msg["gn"].asCString());
    query << cache;
    return !query.exec();
}


int srv_getgm(Json::Value msg, string &store) {
    Query query = dbconn.query();
    sprintf(cache, "select gml from users where gn=\"%s\";", msg["gn"].asCString());
    query << cache;

    StoreQueryResult res = query.store();
    if (res.size() == 0)
        return 1;
    store = res[0]["gml"].c_str();
    return 0;
}