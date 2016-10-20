/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-10 11:13
* Last modified: 2016-10-10 11:13
* Filename: util.h
* Description: 
**********************************************/

#ifndef CCML_UTILS_UTIL_H
#define CCML_UTILS_UTIL_H

#include <vector>
#include <functional>

namespace ccml{
namespace utils{

void init_main(int argc, char** argv);


void register_init_function(std::function<void()> func, int priority = 0);

class InitFunction{
public:
    explicit InitFunction(std::function<void()> func, int priority = 0){
        register_init_function(func, priority);
    }
};//class InitFunction


}//namespace utils
}//namespace ccml

#endif
