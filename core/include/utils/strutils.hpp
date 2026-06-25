#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

namespace yoyo::utils::str {
    // clone a string and convert it to lowercase.
    inline std::string to_lower(const std::string& source) {
        std::string copy = source;
        std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return copy;
    }

    // Join a vector<string> into a string with a delimiter
    inline std::string join(const std::vector<std::string>& v) {
        std::string res;
        for (size_t i = 0; i < v.size(); i++) {
            res += v[i];
            if (i < v.size() - 1) {
                res += "\n";
            }
        }
        return res;
    }
};
