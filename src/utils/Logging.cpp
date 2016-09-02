##############################################
# Author: Jun Jiang - jiangjun4@sina.com
# Last modified:	2016-08-31 15:02
# Filename:		Logging.cpp
# Description: 
##############################################

#include "Logging.h"

namespace ccml{
void initialize_logging(int argc, char* argv[]){
    (void)(argc);
    if(!getenv("GLOG_logtostderr")){
        google::LogToStderr();
    }
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
}

namespace logging{
    void install_failure_function(void (*callback)()){
        google::InstallFailureFunction(callback);
    }
    void install_failure_writer(void (*callback)(const *char, int)){
        google::InstallFailureWriter(callback);
    }
}//namespace logging
}//namespace ccml
