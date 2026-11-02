#include "../include/Adapter.h"
#include <string>
#include <algorithm>
using namespace std;

//int LegacySystemApi::counter = 0;

string LegacySystemApi::submitRecord(string record) {
    int n = ++counter;
    if(record.empty() || n % 10 == 0) {
        return "ERR";
    }

    string num = to_string(n);
    string code = "OK" + string(4 - min<size_t>(4, num.size()), '0') + num;
    lookups[code] = 0;

    return code;
}

string LegacySystemApi::lookup(string code) {
    auto it = lookups.find(code);
    if(it == lookups.end()) return "99";

    
    int& times = it->second;      
    string status = times == 0 ? "00" : times == 1 ? "01" : "02";
    if (times < 2) times++;
    return status;

}