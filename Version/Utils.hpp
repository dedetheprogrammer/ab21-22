#pragma once
#include <algorithm>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <time.h>

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// -- Generates a timestamp based on the current machine local time.
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string timestamp() {
    time_t t; std::time(&t);
    char ts[256];
    std::strftime(ts, sizeof ts, "%X %Z, %d-%m-%Y", std::localtime(&t));
    return ts;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// -- Tokenizes a string with regex and the given pattern.
//  s: string to tokenize.
//  pattern: pattern to apply.
//  extrict: removes duplicated and white-space entries.
//  keep: keeps the delimiter of the pattern.
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<std::string> tokenize(std::string s, const std::string pattern, bool extrict, bool keep = false) {
    std::regex re(pattern);
    if (keep) {
        std::sregex_token_iterator b(s.begin(), s.end(), re, {-1,0});
        std::vector<std::string> t;
        remove_copy_if(b, std::sregex_token_iterator(), back_inserter(t), 
                [](std::string const &s) { return s.empty(); });
        return t;
    } else {
        std::sregex_token_iterator b(s.begin(), s.end(), re, -1), e;
        if (extrict) {
            std::vector<std::string> t;
            remove_copy_if(b, std::sregex_token_iterator(), back_inserter(t), 
                    [](std::string const &s) { return s.empty(); });
            return t;
        } else return {b, e};
    }
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// -- Padds the string for n to the left with given char.
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string padding(int i, char c, int n) {
    std::string s = std::to_string(i);
    return std::string(n-s.length(), c).append(s);
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// -- Obtains substring from first ocurrence of pattern and modifies
// -- original string.
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string substr(std::string& s, std::string pattern) {
    int fof = s.find_first_of(pattern);
    if (fof != std::string::npos) {
        std::string sub = s.substr(0, fof);
        s = s.substr(fof+1);
        return sub;
    } else return "";
}
