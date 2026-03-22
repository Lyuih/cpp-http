#include "HttpRequest.h"

bool HttpRequest::parse(Buffer &buffer)
{
    bool ok = true;
    const std::string sep = "\r\n"; // 分隔符
    const int sep_size = sep.size();
    while (state_ != ParseState::FINISH)
    {
        // 查找每一行 \r\n
        const char *begin = std::search(buffer.peek(), buffer.peek() + buffer.readableBytes(), sep.begin(), sep.end());
        // 没有找到\r\n,当前也不处于正文，数据没有接受完
        if (begin == buffer.peek() + buffer.readableBytes() && state_ != ParseState::BODY)
        {
            return false;
        }

        switch (state_)
        {
        case ParseState::REQUEST_LINE:
        {
            std::string req_line(buffer.peek(), begin);
            if (parseRequestLine(req_line))
            {
                buffer.retrieve(req_line.size() + sep_size);
                // 改变状态
                state_ = ParseState::HEADERS;
            }
            else
            {
                ok = false;
            }
        }

        break;
        case ParseState::HEADERS:
        {
            std::string req_head(buffer.peek(), begin);
            if (req_head.empty())
            {
                // 遇到空行，判断是否存在正文
                if (content_length_ > 0)
                {
                    // 存在正文
                    state_ = ParseState::BODY;
                }
                else
                {
                    state_ = ParseState::FINISH;
                }
            }
            else
            {
                parseHeader(req_head);
                buffer.retrieve(req_head.size() + sep_size);
            }
        }
        break;
        case ParseState::BODY:
            if (buffer.readableBytes() >= content_length_)
            {
                // 正文完整
                body_.append(buffer.peek(), content_length_);
                buffer.retrieve(content_length_);
                state_ = ParseState::FINISH;
            }
            else
            {
                // 不完整
                return false;
            }
            break;
        }
        if (ok == false)
        {
            break;
        }
    }
    return ok;
}

bool HttpRequest::parseRequestLine(const std::string &line)
{
    // GET /user?name=john&age=30 HTTP/1.1
    // 1. 利用流字符串拆出三原组数据
    std::stringstream ss(line);
    ss >> method_ >> path_ >> version_;
    // 2. 拆除请求参数
    size_t pos = path_.find('?');
    if (pos != std::string::npos)
    {
        std::string query_str = path_.substr(pos + 1);
        query_string_ = query_str;
        path_ = path_.substr(0, pos);
        // 3.解析query_str
        parseQueryParams(query_str);
    }
    return true;
}

void HttpRequest::parseQueryParams(const std::string query_str)
{
    // name=john&age=30
    const char sep = '&';
    std::stringstream ss(query_str);
    std::string pair;
    while (std::getline(ss, pair, sep))
    {
        // pair = name=john
        auto pos = pair.find('=');
        if (pos != std::string::npos)
        {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            query_params_[key] = value;
        }
    }
}

void HttpRequest::parseHeader(const std::string &line)
{
    /*
    Host: www.example.com
    */
    const std::string sep = ": ";
    const int sep_size = sep.size();
    auto pos = line.find(sep);
    if (pos != std::string::npos)
    {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + sep_size);
        request_head_[key] = value;
    }
}