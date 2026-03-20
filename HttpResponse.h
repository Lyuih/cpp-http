/**
 * @file    HttpResponse.h
 * @brief   负责解析 socket recv 接收到的 HTTP 请求字符串
 * @author  yui
 * @date    2026-03-18
 *
 * @history
 * 2026-03-18  初次创建
 * 2026-03-20  核心功能完成
 */

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <unordered_map>

#include "Logger.h"

class HttpResponse
{
public:
    explicit HttpResponse(const int status = 200)
    :status_(status),
    content_type_("text/html")
    {}
    void setStatus(const int status) {status_ = status;}
    void setContentType(const std::string& content_type) {content_type_ = content_type;}
    void setBody(const std::string& body) {body_ = body;}
    void appendHeader(const std::string& key,const std::string& value) {headers_[key] = value;}
    std::string toString() const ;
private:
    std::string getStatusMsg(int code) const ;
private:
    //响应状态
    int status_;
    //消息类型
    std::string content_type_;
    //请求头
    std::unordered_map<std::string,std::string> headers_;
    //正文
    std::string body_;
};

#endif