#include "Buffer.h"

bool Buffer::ensureWriteableBytes(size_t len)
{
    if (len >= writeableBytes())
    {
        makeSpace(len);
    }
    return true;
}
// |********R<----------Rable----------->W<------Wable-------->|
void Buffer::makeSpace(size_t len)
{
    // 可写空间+之前的碎片空间
    if (writeableBytes() + read_index_ < len)
    {
        // 不够,扩容
        size_t readable_bytes = readableBytes();
        std::memmove(&buffer_[0], &buffer_[read_index_], readable_bytes);
        read_index_ = 0;
        write_index_ = readable_bytes;

        buffer_.resize(write_index_ + len);
    }
    else
    {
        // 足够，内部拷贝把现有数据拷贝到最前面
        size_t readable_bytes = readableBytes();
        // std::copy(&buffer_[read_index_],&buffer_[write_index_],&buffer_[0]);
        std::memmove(&buffer_[0], &buffer_[read_index_], readable_bytes);
        read_index_ = 0;
        write_index_ = readable_bytes;
    }
}

// 追加数据到缓冲区
void Buffer::append(const char *data, size_t len)
{
    // 检测当前可写空间是否足够
    ensureWriteableBytes(len);
    std::copy(data, data + len, &buffer_[write_index_]);
    write_index_ += len;
}

// 移动读指针
void Buffer::retrieve(size_t len)
{
    if (len < readableBytes())
    {
        read_index_ += len;
    }
    else
    {
        read_index_ = 0;
        write_index_ = 0;
    }
}

// 从fd中读取数据到缓冲区
ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    // 创建一个额外的临时空间,64k
    char tmp_buf[65536];
    struct iovec vec[2];
    const size_t writeable = writeableBytes();
    // 第一部分指向buffer
    vec[0].iov_base = &buffer_[write_index_];
    vec[0].iov_len = writeable;
    // 第二部分指向临时空间
    vec[1].iov_base = tmp_buf;
    vec[1].iov_len = sizeof(tmp_buf);
    // 如果当前可写空间足够大，就不需要第二部分
    const int iovcnt = (writeable < sizeof(tmp_buf)) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writeable)
    {
        write_index_ += n;
    }
    else
    {
        // 数据写入了第二部分
        write_index_ = buffer_.size();
        append(tmp_buf, n - writeable);
    }
    return n;
}