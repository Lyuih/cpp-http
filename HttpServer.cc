#include "HttpServer.h"

HttpServer::HttpServer(const int port, const int thread_num)
    : port_(port),
      thread_num_(thread_num)
{
    TcpServer::getInstance();
    TcpServer::getInstance().init(port_, thread_num_, std::bind(&HttpServer::onMessage, this, std::placeholders::_1));
    if (TcpServer::getInstance().createSocket() != TcpServer::TCP_STATUS::OK)
    {
        LOG_FATAL("创建并初始化套接字失败，服务器启动终止");
        ::exit(1);
    }
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

    ssize_t n = buffer.readFd(fd, &err);

    if (n == 0)
    {
        // 正常的 TCP 四次挥手，对端主动断开，不需要报 WARNING
        LOG_DEBUG("客户端正常断开连接 fd:%d", fd);
        ::close(fd);
        return;
    }
    else if (n < 0)
    {
        // 读取错误
        LOG_ERROR("读取数据错误 fd:%d, errno:%d", fd, err);
        ::close(fd);
        return;
    }
    // 2.解析数据
    HttpRequest http_request;
    if (http_request.parse(buffer))
    {
        // 3.执行业务逻辑
        handleRequest(fd, http_request);
        // 重置，否则 epoll 再也不会报这个 fd 的事件。
        TcpServer::getInstance().updateFd(fd, EPOLLIN | EPOLLRDHUP | EPOLLONESHOT);
    }
    else
    {
        // 如果数据不足，我们也要重置监听位，否则之后补发的内容也没法触发下一次 onMessage
        TcpServer::getInstance().updateFd(fd, EPOLLIN | EPOLLRDHUP | EPOLLONESHOT);
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
    if (req.path().find("/cgi-bin/") != std::string::npos)
    {
        handleCgiRequest(fd, req);
    }
    else
    {

        HttpResponse http_response = handleStaticFile(req.path());
        std::string data = http_response.toString();
        TcpServer::getInstance().sendMsg(fd, data);
    }
}

// 处理静态问文件
HttpResponse HttpServer::handleStaticFile(const std::string &path)
{
    HttpResponse res;
    // 2.防御路径穿越攻击 (防 /../)
    if (path.find("..") != std::string::npos)
    {
        LOG_DEBUG("发现非法路径访问:%s", path.c_str());
        res.setStatus(403);
        res.setBody("<h>403 Forbidden</h>");
        return res;
    }

    std::string full_path = "./wwwroot";
    if (path == "/" || path == "")
    {
        full_path += "/index.html";
    }
    else
    {
        full_path += path;
    }
    // 3.尝试以二进制方式打开文件
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
    auto pos = full_path.find_last_of('.');
    std::string suffix;
    if (pos != std::string::npos)
    {
        suffix = full_path.substr(pos);
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

// CGI机制
void HttpServer::handleCgiRequest(int fd, HttpRequest &req)
{
    // 1.创建两个管道
    int pipe_fd_1[2], pipe_fd_2[2];
    if (pipe(pipe_fd_1) < 0)
    {
        LOG_FATAL("匿名管道创建错误：%s", strerror(errno));
        return;
    }
    if (pipe(pipe_fd_2) < 0)
    {
        LOG_FATAL("匿名管道创建错误：%s", strerror(errno));
        return;
    }
    // 2.fork子进程
    pid_t id = fork();
    if (id < 0)
    {
        LOG_FATAL("子进程创建错误：%s", strerror(errno));
        return;
    }
    if (id == 0)
    {
        alarm(5);
        // 1.重定向，把标准输入重定向到管道的读端
        // 标准输出重定向到管理的写端
        ::dup2(pipe_fd_1[0], 0);
        ::dup2(pipe_fd_2[1], 1);

        // 2/关闭所有原始管道fd
        ::close(pipe_fd_1[0]);
        ::close(pipe_fd_2[0]);
        ::close(pipe_fd_1[1]);
        ::close(pipe_fd_2[1]);
        // 3.设置环境变量
        ::setenv("REQUEST_METHOD", req.method().c_str(), 1);
        if (req.method() == "GET")
        {
            setenv("QUERY_STRING", req.query_string().c_str(), 1);
        }
        else if (req.method() == "POST")
        {
            setenv("CONTENT_LENGTH", std::to_string(req.content_length()).c_str(), 1);
        }
        // 4.进程程序替换
        std::string full_path = req.path();
        if (full_path.front() == '/')
        {
            full_path = "." + full_path;
        }
        ::execl(full_path.c_str(), full_path.c_str(), nullptr);
        // 发生错误退出
        // LOG_ERROR("execl 失败，path: %s, errno: %d", full_path.c_str(), errno);
        ::_exit(1);
    }

    // 父进程
    ::close(pipe_fd_1[0]); // 关闭读端
    ::close(pipe_fd_2[1]); // 关闭写端

    // 1.发生post数据
    if (req.method() == "POST")
    {
        // 将写端设置为非阻塞
        int flags = fcntl(pipe_fd_1[1], F_GETFL, 0);
        fcntl(pipe_fd_1[1], F_SETFL, flags | O_NONBLOCK);

        size_t total_written = 0;
        const std::string &body = req.body();
        while (total_written < body.size())
        {
            ssize_t ret = ::write(pipe_fd_1[1], body.c_str() + total_written, body.size() - total_written);
            if (ret > 0)
            {
                total_written += ret;
            }
            else if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                // 管道满了，稍微让出 CPU 或等一会儿，防止死锁
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                break; // 真实错误
            }
        }
    }
    ::close(pipe_fd_1[1]); // 写完后立即关闭，子进程读到 EOF

    // 2.读取子进程的执行结果
    std::string cgi_res;
    char buf[4096];
    ssize_t s = 0;
    while ((s = ::read(pipe_fd_2[0], buf, sizeof(buf) - 1)) > 0)
    {
        buf[s] = '\0';
        cgi_res += buf;
    }
    ::close(pipe_fd_2[0]);

    // 等待子进程结束
    int status = 0;
    ::waitpid(id, &status, 0);

    // 封装响应发送
    HttpResponse http_res;
    http_res.setStatus(200);
    http_res.setBody(cgi_res);
    TcpServer::getInstance().sendMsg(fd, http_res.toString());
}