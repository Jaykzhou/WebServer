// #pragma once

// #include <mutex>
// #include <string>
// #include <thread>
// #include <sys/time.h>
// #include <string.h>
// #include <stdarg.h>           // vastart va_end
// #include <assert.h>
// #include <sys/stat.h>         //mkdir
// #include "blockqueue.h"
// #include "../buffer/buffer.h"

// class Log{
// public:
//     void init(int level, const char* path = "./log",
//                 const char* suffix = ".log",
//                 int maxQueueCapacity = 1024);
//     static Log* Instance();
//     static void FlushLogThread();

//     void write(int level, const char* format, ...);
//     void flush();

//     int GetLevel();
//     void SetLevel(int level);
//     bool IsOpen(){return isOpen_;}
// private:
// //将其声明为私有，是为了单例模式
//     Log();
//     void AppendLogLevelTitle_(int level);
//     virtual ~Log();
//     void AsyncWrite_();

// private:
//     static const int LOG_PATH_LEN = 256;
//     static const int LOG_NAME_LEN = 256;
//     static const int MAX_LINES = 50000;

//     const char* path_;
//     const char* suffix_;

//     int MAX_LINES_;

//     int lineCount_;
//     int toDay_;

//     bool isOpen_;

    
//     Buffer buff_;
//     int level_;
//     bool isAsync_;

//     FILE* fp_;
//     std::unique_ptr<BlockDeque<std::string>> deque_; 
//     std::unique_ptr<std::thread> writeThread_;
//     std::mutex mtx_;
// };
// // 1、#用来把参数转换成字符.

// // 2、##这个运算符把两个语言符号组合成单个语言符号

// // 3、 __VA_ARGS__ 是一个可变参数的宏,实现思想就是宏定义中参数列表的最后一个参数为省略号（也就是三个点）。

// // 4、##__VA_ARGS__ 宏前面加上##的作用在于，当可变参数的个数为0时，这里的##起到把前面多余的","去掉的作用,否则会编译出错。

// // 5、注意宏定义连接符  \  后面不要有任何操作，直接回车，下一行的前面可以有空格。


// //如果用可变参模板呢？这个是不是可以考虑一下
// #define LOG_BASE(level, format, ...)\
//     do{\
//         Log* log = Log::Instance();\
//         if(log->IsOpen() && log->GetLevel() <= level) {\
//             log->write(level, format, ##__VA_ARGS__);\
//             log->flush();\
//         }\
//     }while(0);

// #define LOG_DEBUG(format, ...) do{LOG_BASE(0, format, ##__VA_ARGS__);} while(0);
// #define LOG_INFO(format, ...) do{LOG_BASE(1, format, ##__VA_ARGS__);} while(0);
// #define LOG_WARN(format, ...) do{LOG_BASE(2, format, ##__VA_ARGS__);} while(0);
// #define LOG_ERROR(format, ...) do{LOG_BASE(3, format, ##__VA_ARGS__);} while(0);
/*
 * @Author       : mark
 * @Date         : 2020-06-16
 * @copyleft Apache 2.0
 */ 
#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "blockqueue.h"
#include "../buffer/buffer.h"

class Log {
public:
    void init(int level, const char* path = "./log", 
                const char* suffix =".log",
                int maxQueueCapacity = 1024);

    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char *format,...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }
    
private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* path_;
    const char* suffix_;

    int MAX_LINES_;

    int lineCount_;
    int toDay_;

    bool isOpen_;
 
    Buffer buff_;
    int level_;
    bool isAsync_;

    FILE* fp_;
    std::unique_ptr<BlockDeque<std::string>> deque_; 
    std::unique_ptr<std::thread> writeThread_;
    std::mutex mtx_;
};

#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif //LOG_H