#include "HttpServer.h"

HttpServer::HttpServer(const int port, const int thread_num)
    : port_(port),
      thread_num_(thread_num)
{
    TcpServer::getInstance();
    TcpServer::getInstance().init(port_, thread_num_,std::bind(&HttpServer::onMessage,this,std::placeholders::_1));
    TcpServer::getInstance().createSocket();
}

void HttpServer::loop()
{
    TcpServer::getInstance().loop();
}

// 核心业务处理入口
void HttpServer::onMessage(int fd)
{
    //1.读取数据
    Buffer buffer;
    int err = 0;
    if(buffer.readFd(fd,&err) <=0)
    {
        LOG_WARNING("连接断开或是读取数据错误");
        ::close(fd);
        return;
    }
    //2.解析数据
    HttpRequest http_request;
    if(http_request.parse(buffer))
    {
        //3.执行业务逻辑
        handleRequest(fd,http_request);
    }
    else
    {
        LOG_DEBUG("数据不足");
    }
}

// 根据请求路径分发逻辑
void HttpServer::handleRequest(int fd, HttpRequest &req)
{
    HttpResponse http_response;
    if(req.path() == "/index.html" || req.path() == "/" || req.path() == "")
    {
        http_response.setStatus(200);
        http_response.setBody("<h1>Welcome to My Server!</h1>");
    }
    else
    {
        http_response.setStatus(404);
        http_response.setBody("<h1>404 Not Found</h1>");
    }
    //返回数据
    std::string data = http_response.toString();
    TcpServer::getInstance().sendMsg(fd,data);
}
