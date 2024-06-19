#include "epollServer.h"
#include "responseThreadPool.h"
#include "M_connect.h"

int main() {
    epollServer eps = epollServer(8080);
    [&eps]() { eps.runall(); }();
    return 0;
}