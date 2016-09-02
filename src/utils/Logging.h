##############################################
# Author: Jun Jiang - jiangjun4@sina.com
# Last modified:	2016-08-31 14:43
# Filename:		Logging.h
# Description:log header file 
##############################################

#ifndef CCML_UTILS_LOGGING_H
#define CCML_UTILS_LOGGING_H

#include "glog/logging.h"

namespace ccml{
void initialize_logging(int argc, char* argv[]);
namespace logging{
    void set_min_log_level(int level);
    void install_failure_function(void (*callback)());
    void install_failure_writer(void (*callback)(const char*,int));
}//end logging
}//end ccml
#endif //CCML_UTILS_LOGGING_H
