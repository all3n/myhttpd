#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
using namespace std;

class utils{
    public:
        static void split(const string &line,string tok,vector<string> &out);
        static string & rstrip(string &line,string tok);


};
#endif
