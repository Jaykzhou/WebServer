#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>

//可以用std::chrono 来代替

template<class T>
class BlockDeque{
public:
    explicit BlockDeque(size_t MaxCapacity = 1000);

    ~BlockDeque();

    void clear();
    bool empty();
    bool full();
    void Close();

    size_t size();
    size_t capacity();

    T front();
    T back();

    void push_back(const T& item);
    void push_front(const T& item);
    
    bool pop(T& item);
    bool pop(T& item, int timeout);

    void flush();

private:
    //这个是不需要进行初始化吗？只需要声明一个变量就可以了吗？
    std::deque<T> deq_;
    size_t capacity_;

    //为什么要加互斥量？用于多线程之间的竞争？
    std::mutex mtx_;
    bool isClose_;
    std::condition_variable condConsumer_;
    std::condition_variable condProducer_;
};


template<class T>
BlockDeque<T>::BlockDeque(size_t MaxCapacity):capacity_(MaxCapacity){
    assert(MaxCapacity > 0);
    isClose_ = false;
}

template<class T>
BlockDeque<T>::~BlockDeque(){
    Close();
}

template<class T>
void BlockDeque<T>::Close(){
    //临界区中对该对象中的内容进行操作,互斥量保护的是临界区的所有内容
    {
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
        isClose_ = true;
    }
    //信号量传递消息，用于判断什么？
    condConsumer_.notify_all();
    condProducer_.notify_all();
}

template<class T>
void BlockDeque<T>::flush(){
    std::lock_guard<std::mutex> locker(mtx_);
    //信号量是用来通知哪个
    condConsumer_.notify_one();
}

template<class T>
void BlockDeque<T>::clear(){
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
}

template<class T>
T BlockDeque<T>::front(){
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.front();
}

template<class T>
T BlockDeque<T>::back(){
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.back();
}

template<class T>
size_t BlockDeque<T>::size(){
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}

template<class T>
size_t BlockDeque<T>::capacity(){
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}


template<class T>
void BlockDeque<T>::push_back(const T& item){
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_){
        condProducer_.wait(locker);
    }
    deq_.push_back(item);
    condConsumer_.notify_one();
}

template<class T>
void BlockDeque<T>::push_front(const T& item){
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_){
        condProducer_.wait(locker);
    }
    deq_.push_front(item);
    condConsumer_.notify_one();
}

template<class T>
bool BlockDeque<T>::empty() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.empty();
}

//这时候 item相当于回调参数吧，用于保存弹出的内容
template<class T>
bool BlockDeque<T>::pop(T& item){
    //为什么是这个lock呢？
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        condConsumer_.wait(locker);
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

template<class T>
bool BlockDeque<T>::pop(T& item, int timeout){
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        if(condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) 
            == std::cv_status::timeout){
                return false;
            }
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

/*
 * @Author       : mark
 * @Date         : 2020-06-16
 * @copyleft Apache 2.0
//  */ 
// #ifndef BLOCKQUEUE_H
// #define BLOCKQUEUE_H

// #include <mutex>
// #include <deque>
// #include <condition_variable>
// #include <sys/time.h>

// template<class T>
// class BlockDeque {
// public:
//     explicit BlockDeque(size_t MaxCapacity = 1000);

//     ~BlockDeque();

//     void clear();

//     bool empty();

//     bool full();

//     void Close();

//     size_t size();

//     size_t capacity();

//     T front();

//     T back();

//     void push_back(const T &item);

//     void push_front(const T &item);

//     bool pop(T &item);

//     bool pop(T &item, int timeout);

//     void flush();

// private:
//     std::deque<T> deq_;

//     size_t capacity_;

//     std::mutex mtx_;

//     bool isClose_;

//     std::condition_variable condConsumer_;

//     std::condition_variable condProducer_;
// };


// template<class T>
// BlockDeque<T>::BlockDeque(size_t MaxCapacity) :capacity_(MaxCapacity) {
//     assert(MaxCapacity > 0);
//     isClose_ = false;
// }

// template<class T>
// BlockDeque<T>::~BlockDeque() {
//     Close();
// };

// template<class T>
// void BlockDeque<T>::Close() {
//     {   
//         std::lock_guard<std::mutex> locker(mtx_);
//         deq_.clear();
//         isClose_ = true;
//     }
//     condProducer_.notify_all();
//     condConsumer_.notify_all();
// };

// template<class T>
// void BlockDeque<T>::flush() {
//     condConsumer_.notify_one();
// };

// template<class T>
// void BlockDeque<T>::clear() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     deq_.clear();
// }

// template<class T>
// T BlockDeque<T>::front() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.front();
// }

// template<class T>
// T BlockDeque<T>::back() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.back();
// }

// template<class T>
// size_t BlockDeque<T>::size() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.size();
// }

// template<class T>
// size_t BlockDeque<T>::capacity() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return capacity_;
// }

// template<class T>
// void BlockDeque<T>::push_back(const T &item) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.size() >= capacity_) {
//         condProducer_.wait(locker);
//     }
//     deq_.push_back(item);
//     condConsumer_.notify_one();
// }

// template<class T>
// void BlockDeque<T>::push_front(const T &item) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.size() >= capacity_) {
//         condProducer_.wait(locker);
//     }
//     deq_.push_front(item);
//     condConsumer_.notify_one();
// }

// template<class T>
// bool BlockDeque<T>::empty() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.empty();
// }

// template<class T>
// bool BlockDeque<T>::full(){
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.size() >= capacity_;
// }

// template<class T>
// bool BlockDeque<T>::pop(T &item) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.empty()){
//         condConsumer_.wait(locker);
//         if(isClose_){
//             return false;
//         }
//     }
//     item = deq_.front();
//     deq_.pop_front();
//     condProducer_.notify_one();
//     return true;
// }

// template<class T>
// bool BlockDeque<T>::pop(T &item, int timeout) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.empty()){
//         if(condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) 
//                 == std::cv_status::timeout){
//             return false;
//         }
//         if(isClose_){
//             return false;
//         }
//     }
//     item = deq_.front();
//     deq_.pop_front();
//     condProducer_.notify_one();
//     return true;
// }

// #endif // BLOCKQUEUE_H