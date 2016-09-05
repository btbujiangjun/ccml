##############################################
# Author: Jun Jiang - jiangjun4@sina.com
# Last modified:	2016-09-05 16:29
# Filename:		Queue.h
# Description: 
##############################################

#ifndef CCML_CORE_QUEUE_H
#define CCML_CORE_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace ccml{
namespace core{
template <class T>
class Queue{
public:
    Queue() : num_elements(0){}
    ~Queue(){}
    void enqueue(const T& element){
        std::unique_lock<std::mutex> lock(_queue_lock);
        _elements.emplace_back(element);
        _elements_cnt++;
        _cv.notify_all();
    }
    void enqueue(T&& element){
        std::unique_lock<std::mutex> lock(_queue_lock);
        _elements.emplace_back(std::move(element));
        _elements_cnt++;
        _cv.notify_all();
    }

    T dequeue(){
        std::unique_lock<std::mutex> lock(_queue_lock);
        _cv.wait(lock,[this](){return _elements_cnt != 0;});

        using std::swap;
        T element;
        swap(_elements.front(), element);

        _elements.pop_front;
        _elements_cnt--;
        if(_elements_cnt == 0){
            _cv.notify_all();
        }
        return element;
    }

    inline int size() const{
        return _elements_cnt;
    }

    inline bool empty() const{
        return _elements_cnt == 0;
    }

    void wait_empty(){
        std::unique_lock<std::mutex> lock(_queue_lock);
        _cv.wait(lock, [this](){return _elements_cnt == 0;});
    }
private:
    std::deque<T> _elements;
    int _element_cnt;
    std::mutex _queue_lock;
    std::condition_variable _cv;
};//class Queue
}//namespace core
}//namespace ccml

#endif
