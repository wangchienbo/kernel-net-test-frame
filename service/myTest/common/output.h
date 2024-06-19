#pragma once
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
class Output {
public:
    Output() {
    }
    string json;
    virtual void unparse() {
    }
    void clear() {
        json = "";
    }
};