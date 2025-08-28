#pragma once

#include <algorithm>
#include <cctype>
#include <ios>
#include <sstream>
#include <string>

inline bool string_to_bool(const std::string& str) noexcept {
    std::istringstream iss(str);
    bool b;
    iss >> std::boolalpha >> b;  // enables reading "true"/"false"
    if (iss.fail()) {
        // Try as integer
        iss.clear();
        iss.str(str);
        int i;
        iss >> i;
        b = (i != 0);
    }
    return b;
}

inline std::string bool_to_string(bool b) noexcept {
    std::ostringstream oss;
    oss << std::boolalpha << b;
    return oss.str();
}

inline void trim(std::string& str) noexcept {
    str.erase(str.begin(),
              std::find_if(str.begin(), str.end(),
                           [](unsigned char ch) { return !std::isspace(ch); }));
    str.erase(std::find_if(str.rbegin(), str.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              str.end());
}
