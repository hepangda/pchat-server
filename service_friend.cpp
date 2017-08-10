#include<service.h>
#include<vector>
#include<algorithm>

using namespace mysqlpp;
using namespace std;
static char cache[10000];//FIXME:TO ENSURE THREAD SAFE!!!!
extern mysqlpp::Connection dbconn;
extern vector<string> OnlineList;

//添加好友的原子操作，若要完整的添加好友，请务必对a添加b，再对b添加a
//@param un 谁
//@oaram afwho 要加谁
//@return 0代表添加成功，1代表添加失败
static int srv_addfriend_atomic(string un, string afwho) {
    Query query = dbconn.query();
    string sql = "select fl from users where un=\"" + un + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value fl;
    j_reader.parse(res[0]["fl"].c_str(), fl);
    if (json_append(fl, "fl", afwho) == -1) {
        return 1;
    }
    string result = j_writer.write(fl);
    json_tosql(result);

    sql = "update users set fl=\"" + result + "\" where un=\"" + un + "\";";
    query << sql;
    return !query.exec();
}

static int srv_delfriend_atomic(string un, string dfwho) {
    Query query = dbconn.query();
    string sql = "select fl from users where un=\"" + un + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value fl;
    j_reader.parse(res[0]["fl"].c_str(), fl);
    int idx = json_findarray(fl, "fl", dfwho);
    if (idx == -1) {
        return 1;
    }
    Json::Value done;
    fl["fl"].removeIndex(idx, &done);
    string result = j_writer.write(fl);
    json_tosql(result);

    sql = "update users set fl=\"" + result + "\" where un=\"" + un + "\";";
    query << sql;
    return !query.exec();
}

//添加好友
//@param msg 添加好友包请求的msg
//@return 0代表成功 1代表失败。
int srv_addfriend(Json::Value msg) {
    if(srv_addfriend_atomic(msg["un"].asString(), msg["afwho"].asString()) != 0) {
        return 1;
    }

    if (srv_addfriend_atomic(msg["afwho"].asString(), msg["un"].asString()) != 0) {
        //FIXME:撤销掉上一次的原子操作
        return 1;
    }
    return 0;
}


int srv_delfriend(Json::Value msg) {
    if(srv_delfriend_atomic(msg["un"].asString(), msg["dfwho"].asString()) != 0) {
        return 1;
    }

    if (srv_delfriend_atomic(msg["dfwho"].asString(), msg["un"].asString()) != 0) {
        //FIXME:撤销掉上一次的原子操作
        return 1;
    }
    srv_delcr_private(msg["un"].asString(), msg["dfwho"].asString());
    return 0;
}

int srv_getfl(Json::Value msg, string &store) {
    Query query = dbconn.query();
    string sql = "select fl from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;

    StoreQueryResult res = query.store();
    if (res.size() == 0)
        return 1;
    store = res[0]["fl"].c_str();
    return 0;
}

int srv_getgl(Json::Value msg, string &store) {
    Query query = dbconn.query();
    string sql = "select gl from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;

    StoreQueryResult res = query.store();
    if (res.size() == 0)
        return 1;
    store = res[0]["gl"].c_str();
    return 0;
}

//@return 2代表被屏蔽，1代表离线，0代表在线
int srv_getfst(string un) {
    Query query = dbconn.query();
    string sql = "select mute from users where un=\"" + un + "\";";
    query << sql;

    StoreQueryResult res = query.store();
    Json::Value mute;
    j_reader.parse(res[0]["mute"].c_str(), mute);
    int ret = json_findarray(mute, "mute", un.c_str());
    if (ret != -1)
        return 2;
    
    if (find(OnlineList.begin(), OnlineList.end(), un) != OnlineList.end())
        return 0;
    return 1;
}

int srv_mute_enable(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "select mute from users where un=\"" + msg["un"].asString() + "\";";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value mute;
    j_reader.parse(res[0]["mute"].c_str(), mute);
    if (json_append(mute, "mute", msg["mtwho"].asString()) == -1) {
        return 1;
    }
    string result = j_writer.write(mute);
    json_tosql(result);

    sql = "update users set mute=\"" + result + "\" where un=\"" + msg["un"].asString() + "\";";
    query << sql;
    return !query.exec();
}

int srv_mute_disable(Json::Value msg) {
    Query query = dbconn.query();
    string sql = "select mute from users where un=\"" + msg["mute"].asString() + "\"";
    query << sql;

    StoreQueryResult res = query.store();

    Json::Value mute;
    j_reader.parse(res[0]["mute"].c_str(), mute);
    int idx = json_findarray(mute, "mute", msg["mtwho"].asCString());
    if (idx == -1) {
        return 1;
    }
    Json::Value done;
    mute["mute"].removeIndex(idx, &done);
    string result = j_writer.write(mute);
    json_tosql(result);

    sql = "update users set mute=\"" + result + "\" where un=\"" + msg["un"].asString() + "\";";

    query << cache;
    return !query.exec();
}