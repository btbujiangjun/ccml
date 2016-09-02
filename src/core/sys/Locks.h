##############################################
# Author: Jun Jiang - jiangjun4@sina.com
# Last modified:	2016-09-01 16:36
# Filename:		Locks.h
# Description: 
##############################################

#ifndef CCML_CORE_SYS_LOCKS_H
#define CCML_CORE_SYS_LOCKS_H

#include <mutex>
#include <condition_variable>

namespace ccml{
namespace core{
namespace sys{

class LockedCondition : public std::condition_variable{
public:
    template <class T>
    void notify_one(T t){
        std::lock_guard<std::mutex> guard(_mutex);
        t();
        std::condition_variable::notify_one();
    }

    template <class T>
    void notify_all(T t){
        std::lock_guard<std::mutex> guard(_mutex);
        t();
        std::condition_variable::notify_all();
    }

    template <class T>
    void wait(Predicate pred){
        std::unique_lock<std::mutex> lock(_mutex);
        std::condition_variable::wait(lock, pred);
    }

    std::mutex* get_mutex(){
        return &_mutex;
    }

protected:
    std::mutex _mutex;
};//class LockedCondition

}//namespace sys
}//namespace core
}//namespace ccml
#endif
