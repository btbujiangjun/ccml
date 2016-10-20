/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-14 15:53
* Last modified: 2016-09-14 15:53
* Filename: ThreadLocal.h
* Description: 
**********************************************/

#ifndef CCML_CORE_THREADLOCAL_H
#define CCML_CORE_THREADLOCAL_H

#include <mutex>
#include <pthread.h>
#include "utils/Logging.h"


namespace ccml{
namespace core{

template <class T>
class ThreadLocal{
public:
    ThreadLocal(){
        /// param2 clear func
        CC_CHECK(pthread_key_create(&_thread_specific_key, data_destructor) == 0);
    }
    ~ThreadLocal(){
        pthread_key_delete(_thread_specific_key);
    }

    T* get(bool create_local = true){
        T* p = (T*)pthread_getspecific(_thread_specific_key);
        if(!p && create_local){
            p = new T();
            CC_CHECK(pthread_setspecific(_thread_specific_key, p) == 0);
        }
        return p;
    }

    void set(T* p){
        if(T* q = get(false)){
            data_destructor(q);
        }
        CC_CHECK(pthread_setspecific(_thread_specific_key, p) == 0);
    }

    T& operator*(){
        return *get();
    }

    operator T*(){
        return get();
    }

private:
    static void data_destructor(void* p){
        delete (T*)p;
    }
    pthread_key_t _thread_specific_key;
};//class ThreadLocal
}//namespace core
}//namespace ccml
#endif
