#include "testApi/apiRun.h"
int main() {
    ApiRun apiRun=ApiRun();
    apiRunResp res=apiRun.run("Api1", "?sd1=1&temp={num=100&abs=10}&sd2=2");
    cout << res.code << endl;
    cout << res.data << endl;
    return 0;
}