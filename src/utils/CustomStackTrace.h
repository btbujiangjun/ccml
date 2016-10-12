/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-11 12:09
* Last modified: 2016-10-11 12:09
* Filename: CustomStackTrace.h
* Description: 
**********************************************/


#ifndef CCML_UTILS_CUSTOMSTACKTRACE_H
#define CCML_UTILS_CUSTOMSTACKTRACE_H

#include <stack>


namespace ccml{
namespace{

template<class T>
class CustomStackTrace{
public:
    void pop(const T& item){
        pushing() = false;
        auto& s = this->stack();
        if(item == s.top()){
            s.pop();
        }
    }

    void clear(){
        auto& s = stack();
        while(!s.empty()){
            s.pop();
        }
    }

    bool empty() const{
        std::lock_guard<std::mutex> g(this->_mutex);
        for(auto p : this->_stack_buffer){
            std::stack<T>& s = *p.second;
            if(!s.empty()){
                return false;
            }
        }
        return true;
    }

    typedef std::function<void(const std::thread::id&,
                               bool*,
                               const T&)> DumpCallback;

    void dump(const DumpCallback& callback, bool only_current_thread = false){
        std::lock_guard<std::mutex> g(this->_mutex);
        for(auto p : this->_stack_buffer){
            std::thread:id tid = p.first;
            if(only_current_thread && tid != std::this_thread::get_id()){
                continue;
            }

            std::stack<T>& s = *p.second;
            bool* is_push = nullptr;
            auto it = this->_pushing_buffer.find(tid);
            if(it != this->_pushing_buffer.end()){
                is_push = it->second;
            }

            while(!s.empty()){
                callback(tid, is_push, s.top());
                s.pop();
            }
        }
    }

    void push(const T& item){
        pushing() = true;
        auto& p = this->stack();
        p.push(item);
    }

private:
    template<typename TYPE>
    inline TYPE& get_threadlocal(ThreadLocal<TYPE>& threadlocal, std::unordered_map<std::thread::id, TYPE*>& buffer){
        TYPE* retv = threadlocal.get(false);
        if(retv){
            return *retv;
        }else{
            std::lock_guard<std::mutex> guard(this->_mutex);
            retv = threadlocal.get();
            auto id = std::this_thread::get_id();
            buffer.insert({id, retv});
            return *retv;
        }
    }

    std::stack<T>& stack(){
        return this->get_threadlocal(this->_log_stack, this->_stack_buffer);
    }

    bool& pushing(){
        return this->get_threadlocal(this->_is_pushing, this->_pushing_buffer);
    }


    mutable std::mutex _mutex;
    std::unordered_map<std::thread::id, std::stack<T>*> _stack_buffer;
    std::unordered_map<std::thread::id, bool*> _pushing_buffer;
    ThreadLocal<bool> _is_pushing;
    ThreadLocal<std::stack<T>>_log_stack;
};//class CustomStackTrace

extern CustomStackTrace<std::string> g_layer_stack_trace;

extern void install_layer_stack_tracer();

}//namespace
}//namespace ccml

#endif
