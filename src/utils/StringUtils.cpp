/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-12 16:42
* Last modified: 2016-10-12 16:42
* Filename: StringUtils.cpp
* Description: 
**********************************************/

#include "StringUtils.h"

namespace ccml{
namespace utils{
namespace str{

bool starts_with(const std::string& str, const std::string& prefix){
    if(prefix.size() <= str.size()){
        for(size_t i = 0; i < prefix.size(); i++){
            if(str[i] != prefix[i]){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

bool ends_with(const std::string& str, const std::string& ext){
    if(str.size() >= ext.size() &&
            ext == str.substr(str.size() - ext.size())){
        return true;
    }else{
        return false;
    }
}

void split(const std::string& str,
           char sep,
           std::vector<std::string>* pieces){
    pieces->clear();
    if(str.empty()){
        return;
    }

    size_t pos = 0;
    size_t next = str.find(sep, pos);
    while(next != std::string::npos){
        pieces->push_back(str.substr(pos, next - pos));
        pos = next + 1;
        next = str.find(sep, pos);
    }
    if(!str.substr(pos).empty()){
        pieces->push_back(str.substr(pos));
    }
}

}//namespace str
}//namespace utils
}//namespace ccml
