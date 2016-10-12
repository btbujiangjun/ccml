/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-12 16:18
* Last modified: 2016-10-12 16:18
* Filename: StringUtils.h
* Description: 
**********************************************/

#pragma once

#include "Logging.h"
#include <string>
#include <vector>
#include <sstream>
#include <typeinfo>

namespace ccml{
namespace utils{
namespace str{

bool starts_with(const std::string& str, const std::string& prefix);
    
bool ends_with(const std::string& str, const std::string& ext);

void split(const std::string& str,
           char sep,
           std::vector<std::string>* pieces);

template<class T>
inline T to_with_status(const std::string& s, bool* ok = nullptr){
    std::istringstream sin(s);
    T v;
    sin >> v;
    if(ok){
        *ok = sin.eof() && !sin.fail();
    }
    return v;
}

template<class T>
inline T to(const std::string& s){
    bool ok;
    T v = to_with_status<T>(s, &ok);
    CC_CHECK(ok) << "Cannot convert s(" << s << ") to type " << typeid(T).name();
    return v;
}

}//namespace str
}//namespace utils
}//namespace ccml
