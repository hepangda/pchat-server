#include<service.h>

using namespace mysqlpp;
using namespace std;
extern mysqlpp::Connection dbconn;

int srv_addgroup(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "insert into groups(gn,gml,gcreater,gmgr) value(\"" + msg["gn"].asString() +
                 "\",\"{\\\"gml\\\":[\\\"" + msg["un"].asString() + "\\\"]}\",\"" +
                 msg["un"].asString() + "\",\"{\\\"gmgr\\\":[]}\")";
    query << sql;

    if (!query.exec())
        return 11;

    //mQuery q2 = dbconn.query();
    sql = "select gl from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;
    StoreQueryResult res = query.store();

    if (res.size() == 0) {
        cerr << "\033[33m" << query.error() << "\033[0m" << endl;
        return 2;
    }

    Json::Value gl;
    j_reader.parse(res[0]["gl"].c_str(), gl);
    if (json_append(gl, "gl", msg["gn"].asString()) == -1) {
        return 3;
    }
    string result = j_writer.write(gl);
    json_tosql(result);

    sql = "update users set gl=\"" + result + "\" where un=\"" + 
          msg["un"].asString() + "\";";

    query << sql;
    return !query.exec();
}

int srv_delgroup(Json::Value msg) {
    int power = srv_getgst(msg["gn"].asString(), msg["un"].asString());
    if (power != 0)
        return 1;
    
    Query query = dbconn.query();
    string sql = "delete from groups where gn=\"" + msg["gn"].asString() + "\";";
    query << sql;
    srv_delcr_group(msg["gn"].asString());
    return !query.exec();
}

//@return 2代表成员，1代表管理员，0代表群主,3代表群不存在
int srv_getgst(string gn, string un) {
    Query query = dbconn.query();
    string sql = "select * from groups where gn=\"" + gn + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    if (res.size() == 0)
        return 3;

    Json::Value gmgr;
    j_reader.parse(res[0]["gmgr"].c_str(), gmgr);
    int ret = json_findarray(gmgr, "gmgr", un.c_str());

    if (ret != -1)
        return 1;

    if (un == string(res[0]["gcreater"].c_str()))
        return 0;

    return 2;
}

int srv_addtogroup(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "select gl from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;
    StoreQueryResult res = query.store();

    if (res.size() == 0)
        return 2;

    Json::Value gl;
    j_reader.parse(res[0]["gl"].c_str(), gl);
    if (json_append(gl, "gl", msg["gn"].asString()) == -1) {
        return 3;
    }
    string result = j_writer.write(gl);
    json_tosql(result);

    sql = "update users set gl=\"" + result + "\" where un=\"" + 
          msg["un"].asString() + "\";";

    query << sql;
    if(!query.exec()) {
        return 4;
    }

    sql = "select gml from groups where gn=\"" + msg["gn"].asString() + "\";";
    query << sql;
    res = query.store();
    
    if (res.size() == 0)
        return 6;

    Json::Value gml;
    j_reader.parse(res[0]["gml"].c_str(), gml);
    if (json_append(gml, "gml", msg["un"].asString()) == -1) {
        return 7;
    }
    result = j_writer.write(gml);
    json_tosql(result);

    sql = "update groups set gml=\"" + result + "\" where gn=\"" +
          msg["gn"].asString() + "\";";

    query << sql;
    return !query.exec();
}

int srv_delfromgroup(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "select gl from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    if (res.size() == 0) {
        return 1;
    }

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

    sql = "update users set gl=\"" + result + "\" where un=\"" + msg["un"].asString() + "\";";
    query << sql;
    if(!query.exec()) {
        return 1;
    }

    sql = "select gml from groups where gn=\"" + msg["gn"].asString() + "\";";
    query << sql;
    res = query.store();
    
    if (res.size() == 0)
        return 1;
        
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

    sql = "update groups set gml=\"" + result + "\" where gn=\"" + msg["gn"].asString() + "\";";
    query << sql;
    return !query.exec();
}


int srv_getgm(Json::Value msg, string &store) {
    Query query = dbconn.query();
    string sql = "select * from groups where gn=\"" + msg["gn"].asString() + "\";";
    query << sql;

    StoreQueryResult res = query.store();
    if (res.size() == 0) {
        cerr << "\033[33m#" << query.error() << "#\033[0m" << endl;
        cerr << "\033[33m#" << sql << "#\033[0m" << endl;
        return 1;
    }
    store = res[0]["gml"].c_str();
    return 0;
}

int srv_setmgr(string gn, string afwho) {
    Query query = dbconn.query();
    string sql = "select gmgr from groups where gn=\"" + gn + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value gmgr;
    j_reader.parse(res[0]["gmgr"].c_str(), gmgr);
    if (json_append(gmgr, "gmgr", afwho) == -1) {
        return 1;
    }
    string result = j_writer.write(gmgr);
    json_tosql(result);

    sql = "update groups set gmgr=\"" + result + "\" where gn=\"" + gn + "\";";
    query << sql;
    return !query.exec();
}

int srv_canmgr(string gn, string dfwho) {
    Query query = dbconn.query();
    string sql = "select gmgr from groups where gn=\"" + gn + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value gmgr;
    j_reader.parse(res[0]["gmgr"].c_str(), gmgr);
    int idx = json_findarray(gmgr, "gmgr", dfwho);
    if (idx == -1) {
        return 1;
    }
    Json::Value done;
    gmgr["gmgr"].removeIndex(idx, &done);
    string result = j_writer.write(gmgr);
    json_tosql(result);

    sql = "update groups set gmgr=\"" + result + "\" where gn=\"" + gn + "\";";
    query << sql;
    return !query.exec();
}

std::string srv_getcrtname(std::string gn) {
    Query query = dbconn.query();
    string sql = "select gcreater from groups where gn=\"" + gn + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    return string(res[0]["gcreater"].c_str());
}