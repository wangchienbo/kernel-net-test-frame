#include "epollServer.h"
#include "responseThreadPool.h"
#include "M_connect.h"

int main() {
    myconnect mc = myconnect("127.0.0.1", 8080);
    cout << "初始化" << endl;
    cout << mc.init() << endl;
    cout << "开始执行\n>>";
    mc.run();
    while (1) {
        string s;
        std::getline(std::cin, s);
        mc.mywrite(s);
        string res;
        if (mc.myread(res) == 1)
            cout << "执行成功\n>>";
        else
            cout << "执行失败，未获取返回值\n>>";
    }
    return 0;
}