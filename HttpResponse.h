#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "Logger.h"

class HttpResponse
{
public:
private:
    //响应状态
    int status_;
    //消息类型
    std::string content_type_;
    //正文
    std::string body;
};

#endif