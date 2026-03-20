#include "HttpResponse.h"

std::string HttpResponse::toString() const
{
    const std::string sep = "\r\n";
    const std::string space = " ";
    const std::string version = "HTTP/1.1";
    // 1.拼装状态行
    std::string status_line = version + space + std::to_string(status_) + space + getStatusMsg(status_) + sep;
    // 2. 拼装响应头
    std::string headers;
    headers += "Content-Type: " + content_type_ + sep;
    headers += "Content-Length: " + std::to_string(body_.size()) + sep;
    headers += "Connection: close\r\n"; // 目前我们先使用短连接
    for (auto &[key, val] : headers_)
    {
        headers.append(key + ": " + val + sep);
    }

    return status_line + headers + sep + body_;
}

std::string HttpResponse::getStatusMsg(int code) const 
{
    switch (code)
    {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 500:
        return "Internal Server Error";
    default:
        return "Unknown";
    }
}