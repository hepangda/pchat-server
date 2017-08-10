#include<string>
#include<json/json.h>

extern Json::Reader j_reader;
extern Json::FastWriter j_writer;

int json_tosql(std::string &st);
int json_append(Json::Value &lt, const std::string apwhat, std::string newitem);
int json_findarray(Json::Value jv, const string apwhat, const string item);