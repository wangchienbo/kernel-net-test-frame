#include "fileTools.h"
int main() {
    std::string s = "asdasd";
    for (int i = 0; i < 10; i++) {
        s += s;
    }
    writeFileContent("test.txt", s, true);
    writeFileContent("test.txt", s, true);
    ThreadGroup::get_instance().wait();
    cout << readFileContent("test.txt");
    return 1;
}