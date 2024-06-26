#include "../common/http/http.h"
#include "../common/common.h"
int main(){
    httpClient myclient("127.0.0.1",8080);
    myclient.init();
    myclient.run();
    string json=readFileContent("runCase.json");
    auto resp=myclient.Post("/runCase",json,{});
    cout<<resp<<endl;
    return 0;
}