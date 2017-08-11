#include<service.h>
#include<dpkg.h>
using namespace std;

Json::Value val, v2;
Json::FastWriter wri;

int main() {
	database_init();
	string st;
	pkg_t pkg;
	val["un"]="pangda3";
	val["gn"]="nihaoa";
	srv_addgroup(val);
	pkg.jsdata = "{\"un\":\"pangda2\", \"gn\":\"nihaoa\"}";
	dpkg_refreshgm_request(pkg);
    // val["un"] = "pangda3";
	// val["afwho"] = "pangda2";
	// val["dfwho"] = "pangda2";
	// val["mtwho"] = "pangda2";
 	// val["gn"] = "nihaoa";
	// val["qst"] = 3;
	// val["pwd"] = "pppa";
	// val["ans"] = "ddd";

 	// if (srv_delgroup(val) == 0) {
 	// 	cout << "excited!" << endl;
 	// } else {
	// 	cout << "i am angry" << endl;
	// }
	// cout << st << endl;
    return 0;
}
