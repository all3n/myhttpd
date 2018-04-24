#include "utils.h"
#include <iostream>


void utils::split(const string & line, string tok, vector<string> &info)
{
    size_t pos = line.find(tok);
    size_t start = 0;
    while (string::npos != pos)
    {
        string v = line.substr(start, pos - start);
        info.push_back(v);
        start = pos + tok.size();
        pos = line.find(tok, start);
    }
    if (start != line.length())
    {
        info.push_back(line.substr(start));
    }
}


string & utils::rstrip(string & line, string tok)
{
    size_t start = line.length() - tok.length();
    string endstr = line.substr(start, tok.length());
    if (endstr.compare(tok) == 0)
    {
        line = line.substr(0, start);
    }
    return line;
}
