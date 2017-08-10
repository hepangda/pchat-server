#include<service.h>

using namespace mysqlpp;
using namespace std;

extern mysqlpp::Connection dbconn;

int srv_cr_group(string gn, string content) {
    Query query = dbconn.query();

    string ct = content;
    json_tosql(ct);
    string sql = "insert into groupcr(gn,content) value(\"" + gn +
                    "\",\"" + ct + "\");";

    query << sql;
    return !query.exec();
}

int srv_cr_private(string from, string to, string content) {
    Query query = dbconn.query();

    string ct = content;
    json_tosql(ct);

    string sql = "insert into privatecr(fromun,toun,content) value(\"" +
                    from + "\",\"" + to + "\",\"" + ct + "\");";

    query << sql;
    return !query.exec();
}

int srv_delcr_group(string gn) {
    Query query = dbconn.query();

    string sql = "delete from groupcr where un=\"" + gn + "\";";
    query << sql;

    return !query.exec();
}

int srv_delcr_private(string un1, string un2) {
    Query query = dbconn.query();
    string sql = "delete from privatecr where fromun=\"" + un1 + 
                "\" and toun=\"" + un2 + "\";";
    
    query << sql;
    sql = "delete from privatecr where fromun=\"" + un2 + 
            "\" and toun=\"" + un1 + "\";";
    query << sql;
    return !query.exec();
}