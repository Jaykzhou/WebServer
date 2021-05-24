
#include "buffer.h"

//用于保存数据的buffer
Buffer::Buffer(int initBuffSize):
        buffer_(initBuffSize), readPos_(0), writePos_(0)
        {}

//buffer 中还有多少可以写入
size_t Buffer::WritableBytes() const{
    return buffer_.size() - writePos_;
}    

//buffer 中已经存入了多少内容
size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}

//prependable bytes for buffer
//or 
size_t Buffer::PrependableBytes() const{
    return readPos_;
}

//if need ptr for the begining
const char* Buffer::Peek() const{
    return BeginPtr_() + readPos_;
}

//buffer is std::vector<char> ; it has iterator begin();
const char* Buffer::BeginPtr_() const{
    return &*buffer_.begin();
}

char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

//retrieve buffer size for other use;
void Buffer::Retrieve(size_t len){
    assert(len <= ReadableBytes());
    readPos_ += len;
}

//will be used for ptr;
void Buffer::RetrieveUntil(const char* end){
    assert(Peek() <= end);
    Retrieve(end - Peek());
}

//clear buffer for next use;
void Buffer::RetrieveAll(){
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

//return old data by str
std::string Buffer::RetrieveAllToStr(){
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

//the postion to write for beginning
//only read , cannot write to it
const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writePos_;
}

char* Buffer::BeginWrite() {
    return BeginPtr_() + writePos_;
}


void Buffer::MakeSpace_(size_t len) {
    if(WritableBytes() + PrependableBytes() < len){
        buffer_.resize(writePos_ + len + 1);
    }
    else{
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}
void Buffer::EnsureWriteable(size_t len){
    if(WritableBytes() < len){
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

//already write len size;
//or have used len size;
void Buffer::HasWritten(size_t len){
    writePos_ += len;
}


void Buffer::Append(const std::string &str) {
    Append(str.data(), str.length());
}

void Buffer::Append(const char *str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str+len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const void *data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

//peek : the beginning can be read;
void Buffer::Append(const Buffer &buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

//ssize_t 是什么类型的?
ssize_t Buffer::ReadFd(int fd, int* saveErrno){
    char buff[65535];
    struct iovec iov[2];
    const size_t  writeable = WritableBytes();
    //分散读，保证数据全部读完
    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    //readv的用法，是什么样的？
    const ssize_t len = readv(fd , iov, 2);
    if(len < 0){
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writeable){
        writePos_ += len;
    }
    else{
        writePos_ = buffer_.size();
        Append(buff, len - writeable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int *saveErrno) {
    size_t  readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0 ){
        *saveErrno = errno;
        return len;
    }
    readPos_ += len;
    return len;
}