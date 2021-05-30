//
// Created by azhou on 2021/5/30.
//

#include "heaptimer.h"

void HeapTimer::SwapNode_(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].id] = i;
    ref_[heap_[j].id] = j;
}

bool HeapTimer::siftdown_(size_t index, size_t n) {
    //尝试将index往下调整，根据时间戳
    //n为往下调整的最下界
    assert(index >= 0 && index < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    //小根堆的调整
    size_t i = index;
    size_t j = i *2 +1;
    while(j < n){
        //找到左右孩子中最大的那个
        if(heap_[j] < heap_[j+1]){
            j++;
        }
        //如果父节点本来就小于子节点，无需调整
        if(heap_[i] < heap_[j]){
            break;
        }
        //父节点大于子节点
        SwapNode_(i, j);
        i = j;
        j = i * 2 + 1;
    }
    //判断能否进行下移，如果可以则下移，并返回true
    return i > index;
}
//这是用来干啥的呢？调整根据事件发生的事件，调整顺序，调整为小根堆的形式
void HeapTimer::siftup_(size_t i) {
    assert(i >= 0 && i < heap_.size());
    size_t j = (i - 1) /2;
    while(j > 0){
        if(heap_[j] < heap_[i]){
            break;
        }
        SwapNode_(i, j);
        i = j;
        j = (i -1) /2;
    }
}

void HeapTimer::add(int id, int timeout, const TimeoutCallBack &cb) {
    assert(id >= 0);
    size_t i;
    if(ref_.count(id) == 0){
        // 新节点：堆尾插入，调整堆
        i = heap_.size();
        ref_[id] = i;
        heap_.push_back({id, Clock::now() + MS(timeout), cb});
        siftup_(i);
    }
    else{
        //已有结点：调整堆
        i = ref_[id];
        heap_[i].expires = Clock::now() + MS(timeout);
        heap_[i].cb = cb;
        if(!siftdown_(i, heap_.size())){
            siftup_(i);
        }
    }
}

//index指的是 在heap_中的位置
void HeapTimer::del_(size_t index) {
    //remove index timer from heap_
    assert(index >= 0 && index < heap_.size() && !heap_.empty());
    //将要删除的节点换到队尾，然后调整堆
    size_t i = index;
    size_t n = heap_.size() - 1;
    assert( i <= n);
    if(i < n){
        //先对其进行交换，然后再调整
        SwapNode_(i,n);
        //调整的过程，这时候i是之前的n所代表的节点
        if(!siftdown_(i,n)){
            siftup_(i);
        }
    }
    //删除队尾元素
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}

void HeapTimer::doWork(int id) {
    if(heap_.empty() || ref_.count(id) == 0){
        return ;
    }
    size_t i = ref_[id];
    TimerNode node = heap_[i];
    node.cb();
    del_(i);
}

void HeapTimer::adjust(int id, int timeout) {
    assert(!heap_.empty() && ref_.count(id) > 0);
    heap_[ref_[id]].expires = Clock::now() + MS(timeout);
    siftdown_(ref_[id], heap_.size());
}

//remove the first one
void HeapTimer::pop() {
    assert(!heap_.empty());
    del_(0);
}
void HeapTimer::tick() {
    if(heap_.empty()){
        return;
    }
    while(!heap_.empty()){
        TimerNode node = heap_.front();
        if(std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0){
            break;
        }
        node.cb();
        pop();
    }
}

void HeapTimer::clear() {
    ref_.clear();
    heap_.clear();
}

int HeapTimer::GetNextTick() {
    tick();
    size_t res = -1;
    if(!heap_.empty()){
        res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
        if(res < 0){
            res = 0;
        }
    }
    return res;
}