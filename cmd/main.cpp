#include "../common/http/http.h"
#include "../route/route.h"

int main(int argc, char **argv) {
    int port = 8080;
    if (argc >= 2) {
        port = atoi(argv[1]);
    }
    HttpServer server(port);
    register_(&server);
    server.runall();
}