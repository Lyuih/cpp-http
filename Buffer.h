/**
 * Buffer.h
 * 缓冲区，自动扩容，避免频繁搬运
 * yui
 * 2026-03-18
 * 2026-03-18 初次创建
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#define INTICAPACITY 1024

class Buffer
{
public:
    explicit Buffer(const int init_capacity = INTICAPACITY)
        : buffer_(init_capacity),
          read_index_(0),
          write_index_(0)
    {
    }
    // 可读字节数
    size_t readableBytes() const { return write_index_ - read_index_; }
    // 可写字节数
    size_t writeableBytes() const { return buffer_.size() - write_index_; }
    // 返回可读数据的首地址
    const char *peek() const { return &buffer_[read_index_]; }
    // 移动读指针
    void retrieve(size_t len);
    // 追加数据到缓冲区
    void append(const char *data, size_t len);
    //从fd中读取数据到缓冲区
    ssize_t readFd(int fd,int* saveErrno);

private:
    bool ensureWriteableBytes(size_t len);
    // |********R<----------Rable----------->W<------Wable-------->|
    void makeSpace(size_t len);

private:
    // 数据缓冲区
    std::vector<char> buffer_;
    // 读指针，当前读到哪
    size_t read_index_;
    // 写指针，当前写到哪
    size_t write_index_;
};
#endif