#include "../common/http/http.h"
#include "../common/common.h"
int main() {
    httpClient myclient("127.0.0.1", 8080);
    myclient.init();
    myclient.run();
    std::string x = RESOURCES_DIR;
    string json = readFileContent(x + "/case3.json");
    auto resp = myclient.Post("/addCase", json, {});
    cout << resp << endl;
    return 0;
}