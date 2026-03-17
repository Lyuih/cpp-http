/**
 * Logger.h
 * 日志类实现，支持输出到终端或本地
 * yui
 * 2026-03-17
 * 2026-03-17初次创建
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <fstream>

#include <cstdio>
#include <cstdarg>

/**
 * 单例模式实现日志类
 * 支持输出到终端或本地
 */
class Logger
{

public:
    enum class LOG
    {
        INFO = 0,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    Logger(const Logger &logger) = delete;
    Logger(const Logger &&logger) = delete;
    Logger &operator=(const Logger &logger) = delete;
    Logger &operator=(const Logger &&logger) = delete;

    static Logger &getInstance()
    {
        static Logger logger;
        return logger;
    }

    void initLogger(const bool flag = false)
    {
        flag_ = flag;
        return;
    }

    void putLog(const LOG log, const std::string &s, const std::string &file, const std::string &line)
    {
        std::string log_level = getLogLevel(log);
        std::string out_string = "[" + log_level + "] [" + file + ":" + line + "] " + s;
        if (!flag_)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            // std::cout<<"["<<log_level<<"] ["<<file<<":"<<line<<"] "<<s<<std::endl;
            std::cout << out_string << std::endl;
        }
        else
        {
            // 输出到本地
            std::lock_guard<std::mutex> lock(mutex_);
            out_string.append("\n");
            std::fstream out_file("./log/log.log", std::ios::app);
            if (!out_file)
            {
                std::cerr << "打开文件失败" << std::endl;
                return;
            }
            out_file << out_string;
            out_file.close();
        }
    }

private:
    Logger()
        : flag_(false)
    {
    }

    std::string getLogLevel(const LOG log)
    {
        switch (log)
        {
        case LOG::INFO:
            return "INFO";
        case LOG::DEBUG:
            return "DEBUG";
        case LOG::WARNING:
            return "WARNING";
        case LOG::ERROR:
            return "ERROR";
        case LOG::FATAL:
            return "FATAL";
        default:
            return "";
        }
        return "";
    }

private:
    // 日志的输出方式。false表示输出到终端
    bool flag_;
    std::mutex mutex_;
};

#define LOG_INFO(format, ...)                                                                     \
    do                                                                                            \
    {                                                                                             \
        char buf[1024];                                                                           \
        snprintf(buf, sizeof(buf), format, ##__VA_ARGS__);                                        \
        Logger::getInstance().putLog(Logger::LOG::INFO, buf, __FILE__, std::to_string(__LINE__)); \
    } while (0)

#define LOG_DEBUG(format, ...)                                                                     \
    do                                                                                             \
    {                                                                                              \
        char buf[1024];                                                                            \
        snprintf(buf, sizeof(buf), format, ##__VA_ARGS__);                                         \
        Logger::getInstance().putLog(Logger::LOG::DEBUG, buf, __FILE__, std::to_string(__LINE__)); \
    } while (0)

#define LOG_WARNING(format, ...)                                                                     \
    do                                                                                               \
    {                                                                                                \
        char buf[1024];                                                                              \
        snprintf(buf, sizeof(buf), format, ##__VA_ARGS__);                                           \
        Logger::getInstance().putLog(Logger::LOG::WARNING, buf, __FILE__, std::to_string(__LINE__)); \
    } while (0)

#define LOG_ERROR(format, ...)                                                                     \
    do                                                                                             \
    {                                                                                              \
        char buf[1024];                                                                            \
        snprintf(buf, sizeof(buf), format, ##__VA_ARGS__);                                         \
        Logger::getInstance().putLog(Logger::LOG::ERROR, buf, __FILE__, std::to_string(__LINE__)); \
    } while (0)

#define LOG_FATAL(format, ...)                                                                     \
    do                                                                                             \
    {                                                                                              \
        char buf[1024];                                                                            \
        snprintf(buf, sizeof(buf), format, ##__VA_ARGS__);                                         \
        Logger::getInstance().putLog(Logger::LOG::FATAL, buf, __FILE__, std::to_string(__LINE__)); \
    } while (0)
    
#endif