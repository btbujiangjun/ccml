/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-10 17:10
* Last modified: 2016-10-10 17:10
* Filename: Version.cpp
* Description: 
**********************************************/

#include "Version.h"

namespace ccml{
namespace utils{
namespace version{

void print_version(std::ostream& os){
#ifndef CCML_VERSION
#define CCML_VERSION "unknown"
#endif
    os << "ccml version: "<< CCML_VERSION << std::endl << std::boolalpha << "\t" << "withGpu:" << version::is_gpu() << std::endl;
}

void print_version(){
    if(FLAGS_version){
        print_version(std::cout);
        exit(0);
    }
}

}//namespace version
}//namespace utils
}//namespace ccml
