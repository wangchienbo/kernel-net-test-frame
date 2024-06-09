#pragma once
#include <string>
std::pair<bool, int> CheckAndConvert(const std::string& s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            return {false, 0};
        }
    }
    return {true, std::stoi(s)};
}