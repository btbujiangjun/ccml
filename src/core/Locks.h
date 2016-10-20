/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-06 20:55
* Last modified: 2016-09-06 20:55
* Filename: Locks.h
* Description: 
**********************************************/

#ifndef CCML_CORE_LOCKS_H
#define CCML_CORE_LOCKS_H

#include <mutex>
#include <condition_variable>

namespace ccml{
namespace core{

/*
 * A simple read-write lock.
 * The RWLock allows a numbers of readers or at most onewriter at any point in time.
 * The RWlock disable copy.
 */
class RWLock{
public:
     RWLock(){
        pthread_rwlock_init(&_rwlock, NULL);
     }
     RWLock(const RWLock&) = delete;
     RWLock& operator=(const RWLock&) = delete;

     ~RWLock(){
        pthread_rwlock_destroy(&_rwlock);
     }

     /*
      * lock on write mode.
      * the method will block the thread, if failed to get the lock.
      */
     void lock(){
        pthread_rwlock_wrlock(&_rwlock);
     }

     /*
      * lock on read mode.
      * if another thread is writing, it can't get the lock, and will block the thread.
      */
     void lock_shared(){
         pthread_rwlock_rdlock(&_rwlock);
     }

     void unlock(){
        pthread_rwlock_unlock(&_rwlock);
     }

protected:
     pthread_rwlock_t _rwlock;
};//class RWLock

class ReadLockGuard{
public:
    explicit ReadLockGuard(RWLock& rwlock) : _rwlock(&rwlock){
        _rwlock->lock_shared();
    }

    ~ReadLockGuard(){
        _rwlock->unlock();
    }
private:
    RWLock* _rwlock;
}; //class ReadLockGuard

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
    void wait(T t){
        std::unique_lock<std::mutex> lock(_mutex);
        std::condition_variable::wait(lock, t);
    }

    std::mutex* get_mutex(){
        return &_mutex;
    }

protected:
    std::mutex _mutex;
};//class LockedCondition

}//namespace core
}//namespace ccml
#endif
