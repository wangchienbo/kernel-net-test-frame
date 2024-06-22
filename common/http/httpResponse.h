#pragma once
#include "../epoll/response.h"
const int HTTP_OK = 200;
const int HTTP_NOT_FOUND = 404;
const int HTTP_INTERNAL_SERVER_ERROR = 500;
const int HTTP_BAD_REQUEST = 400;
class HttpResponse : public Response{
};