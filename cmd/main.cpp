#include "../common/http/http.h"
#include "../route/route.h"
int main() {
    HttpServer server(8080);
    register_(&server);
    server.runall();
}