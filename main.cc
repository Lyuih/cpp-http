#include <chrono>
#include <ctime>
#include <cstdlib>

#include "Logger.h"
#include "TcpServer.h"
#include "HttpServer.h"

// void test()
// {
//     std::cout<<"开始执行任务"<<std::endl;
// }

int main()
{
    Logger::getInstance().initLogger();
    HttpServer http_server(8888);
    http_server.loop();

    // srand((unsigned int)time(nullptr));
    // ThreadPool thread_pool(5);
    // thread_pool.start();
    // for (;;)
    // {
    //     // 添加任务
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     LOG_DEBUG("开始添加任务");
    //     thread_pool.append(test);
    // }
    // thread_pool.shutdown();

    return 0;
}