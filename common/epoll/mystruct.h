#pragma once
#include "head.h"
struct my_in_addr
{
    u_int32_t ipv4_addr;/* data */
};
struct my_sockaddr_in{
    sa_family_t sin_family;
    u_int16_t sin_port;
    struct my_in_addr sin_addr;
};
