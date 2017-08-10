#include<json_assist.h>

using namespace std;
Json::Reader j_reader;
Json::FastWriter j_writer;

int json_tosql(string &st) {
	for (auto i = 0u; i < st.length(); i++) {
		if (st[i + 1] == '\"') {
			st.insert(i + 1, "\\");
			i++;
		}
	}
	st[st.length() - 1] = '\0';
}

int json_append(Json::Value &lt, const string apwhat, string newitem) {
	for (auto i = 0u; i < lt[apwhat].size(); i++) {
		if (lt[apwhat][i] == newitem)
			return -1;
	}

    lt[apwhat].append(newitem);
    return 0;
}

int json_findarray(Json::Value jv, const string apwhat, const string item) {
	for (auto i = 0u; i < jv[apwhat].size(); i++) {
		if (jv[apwhat][i] == item)
			return i;
	}
	return -1;
}