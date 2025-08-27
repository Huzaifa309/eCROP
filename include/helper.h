#pragma once

#include <iostream>
#include <sstream>
#include <string>

inline bool string_to_bool(const std::string& str) noexcept {
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;  // enables reading "true"/"false"
    if (is.fail()) {
        // Try as integer
        is.clear();
        is.str(str);
        int i;
        is >> i;
        return i != 0;
    }
    return b;
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
