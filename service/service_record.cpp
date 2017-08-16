#include<service.h>

using namespace mysqlpp;
using namespace std;

extern mysqlpp::Connection dbconn;

int srv_cr_group(string gn, string un, string content) {
    Query query = dbconn.query();

    string ct = content;
    json_tosql(ct);
    string sql = "insert into groupcr(gn,content,un) value(\"" + gn +
                    "\",\"" + ct + "\",\"" + un + "\");";

    query << sql;
    return !query.exec();
}

int srv_cr_private(string from, string to, string content, int st) {
    Query query = dbconn.query();

    string ct = content;
    json_tosql(ct);

    string sql;
    if (st == 0) {
        sql = "insert into privatecr(fromun,toun,content,hasread) value(\"" +
                    from + "\",\"" + to + "\",\"" + ct + "\",0);";
    } else {
        sql = "insert into privatecr(fromun,toun,content) value(\"" +
                    from + "\",\"" + to + "\",\"" + ct + "\");";
    }

    query << sql;
    return !query.exec();
}

int srv_delcr_group(string gn) {
    Query query = dbconn.query();

    string sql = "delete from groupcr where gn=\"" + gn + "\";";
    query << sql;

    return !query.exec();
}

int srv_delcr_private(string un1, string un2) {
    Query query = dbconn.query();
    string sql = "delete from privatecr where fromun=\"" + un1 + 
                "\" and toun=\"" + un2 + "\";";

    query << sql;
    query.exec();
    sql = "delete from privatecr where fromun=\"" + un2 + 
            "\" and toun=\"" + un1 + "\";";
    query << sql;

    return !query.exec();
}

int srv_fetch_grpcr(string gn, string &store) {
    Query query = dbconn.query();
    string sql = "select * from groupcr where gn=\"" + gn + "\";";
    query << sql;
    
    StoreQueryResult res = query.store();
    if (res.size() == 0)
        return 1;
    
    store = "";
    for (auto i = 0u; i < res.size(); i++) {
        store += string(res[i]["un"].c_str()) + ": " + res[i]["content"].c_str() + "\n";
    }
    return 0;
}

int srv_fetch_privatecr(string un1, string un2, string &store) {
    Query query = dbconn.query();
    string sql = "select * from privatecr where fromun=\"" + un1 + 
                "\" and toun=\"" + un2 + "\";";
    query << sql;
    cout << "\033[33m" << sql << "\033[0m" << endl;
    StoreQueryResult res = query.store();
   
    store = "";
    for (auto i = 0u; i < res.size(); i++) {
        store += string(res[i]["fromun"].c_str()) + ": " + res[i]["content"].c_str() + "\n";
    }

    sql = "select * from privatecr where fromun=\"" + un2 + 
                "\" and toun=\"" + un1 + "\";";
    query << sql;
        cout << "\033[33m" << sql << "\033[0m" << endl;
    res = query.store();

    for (auto i = 0u; i < res.size(); i++) {
        store += string(res[i]["fromun"].c_str()) + ": " + res[i]["content"].c_str() + "\n";
    }
    return 0;
}

int srv_fch(std::string un, std::string &store) {
    Query query = dbconn.query();
    string sql = "select * from privatecr where toun=\"" + un + "\" and hasread=0;"; 
    query << sql;
    
    StoreQueryResult res = query.store();
   
    store = "";
    for (auto i = 0u; i < res.size(); i++) {
        store += string(res[i]["fromun"].c_str()) + ": " + res[i]["content"].c_str() + "\n";
    }

    sql = "update privatecr set hasread=1 where toun=\"" + un + "\";";
    query << sql;
    return !query.exec();
}