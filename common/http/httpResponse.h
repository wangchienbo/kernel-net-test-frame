#pragma once
#include "../epoll/response.h"
const int HTTP_OK = 200;
const int HTTP_NOT_FOUND = 404;
const int HTTP_INTERNAL_SERVER_ERROR = 500;
const int HTTP_BAD_REQUEST = 400;
const string SUCCESS = "Success";
const string SERVER_NOT_FOUND = "Server Not Found";
const string INVALID_PARAM = "Invalid Param";
const string NTERNAL_SERVER_ERROR = "Internal Server Error";


map<int, string> code2msg = {
    {HTTP_OK, SUCCESS},
    {HTTP_NOT_FOUND, SERVER_NOT_FOUND},
    {HTTP_INTERNAL_SERVER_ERROR, NTERNAL_SERVER_ERROR},
    {HTTP_BAD_REQUEST, INVALID_PARAM}
};
const string CASE_NOT_EXIST = "Case not exist";
class HttpResponse : public Response{
};