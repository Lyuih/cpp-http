/**
 * @file    HttpRequest.h
 * @brief   负责解析 socket recv 接收到的 HTTP 请求字符串
 * @author  yui
 * @date    2026-03-18
 *
 * @history
 * 2026-03-18  初次创建
 * 2026-03-19  完成http请求解析
 */
#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "Logger.h"
#include "Buffer.h"

class HttpRequest
{
public:
    enum class ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH
    };
    HttpRequest()
        : state_(ParseState::REQUEST_LINE),
          content_length_(0)
    {
    }
    bool parse(Buffer &buffer);
    bool isFinish() const { return state_ == ParseState::FINISH; }
    std::string path(){return path_;}
private:
    bool parseRequestLine(const std::string &line);
    void parseHeader(const std::string &line);
    void parseQueryParams(const std::string query_str);

private:
    // 当前解析状态
    ParseState state_;
    // 正文大小
    size_t content_length_;
    // 方法 GET/POST
    std::string method_;
    // 请求路径
    std::string path_;
    // http版本
    std::string version_;
    // 请求参数
    std::unordered_map<std::string, std::string> query_params_;
    // 请求头
    std::unordered_map<std::string, std::string> request_head_;
    // 正文
    std::string body_;
};

#endif