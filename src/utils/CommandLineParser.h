/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-11 15:07
* Last modified: 2016-10-11 15:07
* Filename: CommandLineParser.h
* Description: 
**********************************************/

#ifndef CCML_UTILS_COMMANDLINEPARSER_H
#define CCML_UTILS_COMMANDLINEPARSER_H

#include "DisableCopy.h"
#include <vector>
#include <string>
#include <stdint.h>

namespace ccml{
namespace utils{

template<typename T>
struct CommandLineFlagRegistry{

    static CommandLineFlagRegistry* instance(){
        static CommandLineFlagRegistry _instance;
        return &_instance;
    }

    struct Command{
        //name of argument
        std::string name;
        //address of actual value. such as FLAGS_XXX
        T* value;
        //usage text
        std::string text;
        //default value of this command
        T default_value;
    };

    std::vector<Command> commands;

    DISABLE_COPY(CommandLineFlagRegistry);

private:
    inline CommandLineFlagRegistry(){}
};

template<typename T>
struct CommandLineFlagRegister{
    CommandLineFlagRegister(const std::string& name,
                            T* value,
                            const std::string desc){
        CommandLineFlagRegistry<T>::instance()->commands.push_back({name, value, desc, *value});
    }
};


#define CC_DEFINE_variable(type, name, default_value, text) \
    type FLAGS_##name = default_value;\
    namespace ccml_utils{\
        ccml::utils::CommandLineFlagRegister<type> flags_var_##name(#name, &FLAGS_##name, text);\
    }//namespace ccml_utils_flags

#define CC_DEFINE_int32(name, default_value, text) \
    CC_DEFINE_variable(int32_t, name, default_value, text)

#define CC_DEFINE_bool(name, default_value, text) \
    CC_DEFINE_variable(bool, name, default_value, text)

#define CC_DEFINE_string(name, default_value, text) \
    CC_DEFINE_variable(std::string, name, default_value, text)

#define CC_DEFINE_double(name, default_value, text) \
    CC_DEFINE_variable(double, name, default_value, text)

#define CC_DEFINE_int64(name, default_value, text) \
    CC_DEFINE_variable(int64_t, name, default_value, text)

#define CC_DEFINE_uint64(name, default_value, text) \
    CC_DEFINE_variable(uint64_t, name, default_value, text)

#define CC_DECLARE_variable(type, name) extern type FLAGS_##name;

#define CC_DECLARE_int32(name) CC_DECLARE_variable(int32_t, name)

#define CC_DECLARE_bool(name) CC_DECLARE_variable(bool, name)

#define CC_DECLARE_string(name) CC_DECLARE_variable(std::string, name)

#define CC_DECLARE_double(name) CC_DECLARE_variable(double, name)

#define CC_DECLARE_int64(name) CC_DECLARE_variable(int64_t, name)

#define CC_DECLARE_uint64(name) CC_DECLARE_variable(uint64_t, name)



void parse_commandline_flags(int* argc,
                             char** argv,
                             bool with_help = true);

}//namespace utils
}//namespace ccml
#endif
