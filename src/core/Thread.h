/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-06 20:56
* Last modified: 2016-09-06 20:56
* Filename: Thread.h
* Description: 
**********************************************/

#ifndef CCML_CORE_THREAD_H
#define CCML_CORE_THREAD_H

#include <thread>
#include "Queue.h"
#include "Locks.h"

namespace ccml{
namespace core{
class Thread{
public:
    Thread(){
        _thread = nullptr;
    }
    virtual ~Thread(){}

    void start(){
        _thread.reset(new std::thread([this](){this->run();}));
    }
    void detach(){
        _thread->detach();
    }
    void join(){
        _thread->join();
    }
    virtual void run() = 0;
protected:
    std::unique_ptr<std::thread> _thread;
};//end Thread

class ThreadWorker : protected Thread{
public:
    typedef std::function<void()> JobFunc;

    ThreadWorker():_stopping(false),_empty(false){
        start();
    }
    ~ThreadWorker(){
        if(!_stopping){
            wait();
            stop();
        }
    }

    void wait(){
        _lc.wait([this]{return _empty;});
    }
    void stop(){
        _stopping = true;
        _jobs.enqueue([](){});
        join();
    }
    void add_job(JobFunc func){
        _empty = false;
        _jobs.enqueue(func);
    }

protected:
    bool _empty;
    bool _stopping;
    LockedCondition _lc;
    Queue<JobFunc> _jobs;

    virtual void run(){
        while(true){
            JobFunc func = _jobs.dequeue();
            if(_stopping){
                break;
            }
            func();
            if(_jobs.empty()){
                _lc.notify_all([this]{_empty = true;});
            }
        }
    }

};//end ThreadWorker

}//namespace core
}//namespace ccml
#endif
