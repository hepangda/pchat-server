#include<service.h>
#include"database_connstr.h"

mysqlpp::Connection dbconn(false);

int database_init() {
    int ret = dbconn.connect(CONNECT_STR);
    return !ret;
}