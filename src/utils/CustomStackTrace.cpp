/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-11 14:38
* Last modified: 2016-10-11 14:38
* Filename: CustomStackTrace.cpp
* Description: 
**********************************************/

#include "CustomStackTrace.h"
#include "CommandLineParser.h"
#include <iostream>

namespace ccml{
namespace utils{

CustomStackTrace<std::string> g_layer_stack_trace;

static std::mutex g_layer_stack_trace_metux;

void install_layer_stack_tracer(){
    logging::install_failure_writer([](const char* data, int size)){
        std::lock_guard<std::mutex> guard(g_layer_stack_trace_metux);
        if(!g_layer_stack_trace.empty()){
            size_t current_tid = -1UL;
            std::hash<std::thread::id> hasher;
            g_layer_stack_trace.dump([&current_tid, &hasher](std::thread::id tid, bool* is_forwarding, const std::string& layer_name){
                if(current_tid != hasher(tid)){
                    if(current_tid != -1UL){
                        std::cerr << std::endl;
                    }
                    current_tid = hasher(tid);
                    std::cerr << "Thread [" << tid << "]";
                    if(is_forwarding){
                        std::cerr << (*is_forwarding ? "Forwarding":"Backwarding");
                    }
                }
                std::cerr << layer_name << ",";
            }, FLAGS_layer_stack_error_only_current_thread);
            std::cerr << std::endl;
    }
    std::cerr.write(data, size);
}

}//namespace utils
}//namespace ccml
