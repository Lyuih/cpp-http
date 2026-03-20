#include "HttpServer.h"

HttpServer::HttpServer(const int port, const int thread_num)
    : port_(port),
      thread_num_(thread_num)
{
    TcpServer::getInstance();
    TcpServer::getInstance().init(port_, thread_num_, std::bind(&HttpServer::onMessage, this, std::placeholders::_1));
    TcpServer::getInstance().createSocket();
}

void HttpServer::loop()
{
    TcpServer::getInstance().loop();
}

// 核心业务处理入口
void HttpServer::onMessage(int fd)
{
    // 1.读取数据
    Buffer buffer;
    int err = 0;
    if (buffer.readFd(fd, &err) <= 0)
    {
        LOG_WARNING("连接断开或是读取数据错误");
        ::close(fd);
        return;
    }
    // 2.解析数据
    HttpRequest http_request;
    if (http_request.parse(buffer))
    {
        // 3.执行业务逻辑
        handleRequest(fd, http_request);
    }
    else
    {
        LOG_DEBUG("数据不足");
    }
}

// 根据请求路径分发逻辑
void HttpServer::handleRequest(int fd, HttpRequest &req)
{
    // HttpResponse http_response;
    // if (req.path() == "/index.html" || req.path() == "/" || req.path() == "")
    // {
    //     http_response.setStatus(200);
    //     http_response.setBody("<h1>Welcome to My Server!</h1>");
    // }
    // else
    // {
    //     http_response.setStatus(404);
    //     http_response.setBody("<h1>404 Not Found</h1>");
    // }
    // // 返回数据
    // std::string data = http_response.toString();
    HttpResponse http_response = handleStaticFile(req.path());
    std::string data = http_response.toString();
    TcpServer::getInstance().sendMsg(fd, data);
}

// 处理静态问文件
HttpResponse HttpServer::handleStaticFile(const std::string &path)
{
    HttpResponse res;
    // 1.查看是否是本地资源
    std::string full_path = "./wwwroot";
    if (path == "/" || path == "")
    {
        full_path += "/index.html";
    }
    else
    {
        full_path += path;
    }
    // 2.尝试以二进制方式打开文件
    std::fstream file(full_path, std::ios::binary | std::ios::in);
    if (!file.is_open())
    {
        // 不存在
        LOG_DEBUG("文件不存在，返回404:%s", full_path.c_str());
        res.setStatus(404);
        res.setBody("<h>404</h>");
        return res;
    }

    // 3.识别后缀并设置content-type
    auto pos = path.find_last_of('.');
    std::string suffix;
    if (pos != std::string::npos)
    {
        suffix = path.substr(pos);
    }
    if (SUFFIX_TYPE.count(suffix))
    {
        res.setContentType(SUFFIX_TYPE.at(suffix));
    }
    else
    {
        // 未知类型使用二进制流
        res.setContentType("application/octet-stream");
    }
    std::stringstream ss;
    ss << file.rdbuf();
    res.setBody(ss.str());
    res.setStatus(200);
    return res;
}