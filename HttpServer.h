/**
 * @file HttpServer.h
 * @brief Http服务器，处理tcp连接，实现http请求和响应
 * @author yui
 * @date 2026-03-16
 *
 * @history
 * 2026-03-16
 * 2026-03-16初次创建
 * 2026-03-20实现http服务器主要功能
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <fstream>
#include <sstream>
#include <cstdlib>
#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <fcntl.h>
#include <sys/wait.h>

/**
 * http服务器
 * 处理tcp连接
 * 负责存储http请求和响应
 */
// 默认端口号
#define DEFAULT_PORT 8889
#define THREAD_NUM 10

static const std::unordered_map<std::string, std::string> SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {".js", "text/javascript"},
};

class HttpServer
{
public:
    HttpServer(const int port = DEFAULT_PORT, const int thread_num = THREAD_NUM);
    // 循环监听获取新连接
    void loop();

    // 核心业务处理入口
    void onMessage(int fd);

private:
    // 处理静态问文件
    HttpResponse handleStaticFile(const std::string& path);
    // 根据请求路径分发逻辑
    void handleRequest(int fd, HttpRequest &req);
    //CGI机制
    void handleCgiRequest(int fd,HttpRequest&req);

private:
    int port_;
    int thread_num_;
};

#endif