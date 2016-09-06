/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Last modified:	2016-08-31 14:43
* Filename:		Logging.h
* Description:log header file 
*********************************************/

#ifndef CCML_UTILS_LOGGING_H
#define CCML_UTILS_LOGGING_H

#include <sstream>

namespace ccml{
namespace logging{

//Log Levels
const int INFO = 0;
const int WARNING = 1;
const int ERROR = 2;
const int FATAL = 3;
const int NUM_SEVERITIES = 4;

class LogMessage : public std::basic_ostringstream<char>{
public:

    LogMessage(const char* name, int line, int severity);
    ~LogMessage();

protected:
    void generate_log_message();

private:
    const char* _name;
    int _line;
    int _severity;
};//class LogMessage

class LogMessageFatal : public LogMessage{
public:
    LogMessageFatal(const char* file, int line) __attribute__((cold));
    ~LogMessageFatal() __attribute__((noreturn));
};//class LogMessageFatal



#define _CC_LOG_INFO \
    ::ccml::logging::LogMessage(__FILE__, __LINE__, ::ccml::logging::INFO)

#define _CC_LOG_WARNING \
    ::ccml::logging::LogMessage(__FILE__, __LINE__, ::ccml::logging::WARNING)

#define _CC_LOG_ERROR \
    ::ccml::logging::LogMessage(__FILE__, __LINE__, ::ccml::logging::ERROR)

#define _CC_LOG_FATAL \
    ::ccml::logging::LogMessage(__FILE__, __LINE__, ::ccml::logging::FATAL)

#define CC_LOG(severity) _CC_LOG_##severity

#define PREDICT_TRUE(x) (__builtin__expect(!!(x), 1))
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))

#define CC_CHECK(condition) \
    if(PREDICT_FALSE(!(condition))) \
    CC_LOG(FATAL) << "Check failed:" #condition " "

#define CC_CHECK_EQ(v1, v2) CC_CHECK((v1) == (v2))
#define CC_CHECK_NE(v1, v2) CC_CHECK((v1) != (v2))
#define CC_CHECK_LE(v1, v2) CC_CHECK((v1) <= (v2))
#define CC_CHECK_LT(v1, v2) CC_CHECK((v1) < (v2))
#define CC_CHECK_GE(v1, v2) CC_CHECK((v1) >= (v2))
#define CC_CHECK_GT(v1, v2) CC_CHECK((v1) > (v2))
#define CC_CHECK_NOTNULL(v) CC_CHECK((v) != NULL)

}//end logging
}//end ccml
#endif //CCML_UTILS_LOGGING_H
