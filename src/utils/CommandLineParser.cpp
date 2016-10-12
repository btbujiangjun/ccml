/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-11 16:41
* Last modified: 2016-10-11 16:41
* Filename: CommandLineParser.cpp
* Description: 
**********************************************/

#include "CommandLineParser.h"
#include "StringUtils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace ccml{
namespace utils{


static constexpr int k_status_ok = 0;
static constexpr int k_status_invalid = 1;
static constexpr int k_status_not_found = 2;

template<typename T>
bool StringToValue(const std::string& content, T* value){
    bool ok;
    *value = str::to_with_status<T>(content, &ok);
    return ok;
}

template<>
bool StringToValue<bool>(const std::string& content, bool* value){
    std::string tmp = content;

    std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](char in)-> char {
        if(in <= 'Z' && in >= 'A'){
            return in - ('Z' - 'z');
        }else{
            return in;
        }
    });

    if(tmp == "true" || tmp == "1"){
        *value = true;
        return true;
    }else if(tmp == "false" || tmp == "0"){
        *value = false;
        return true;
    }else{
        return false;
    }
}

template<>
bool StringToValue<std::string>(const std::string& content, std::string* value){
    *value = content;
    return true;
}


template<typename T>
int parse_argument(const std::string& argument, std::string* extra_info){
    for(auto& command : CommandLineFlagRegistry<T>::instance()->commands){

        std::string& name = command.name;
        T* value = command.value;

        std::string prefix = "--";
        prefix += name;
        prefix += "=";

        std::string content;

        if(str::starts_with(argument, prefix)){
            content = argument.substr(prefix.size(), argument.size() - prefix.size());
        }else{
            prefix = "-";
            prefix += name;
            prefix += "=";
            if(str::starts_with(argument, prefix)){
                content = argument.substr(prefix.size(), argument.size() - prefix.size());
            }
        }
        if(!content.empty()){
            if(StringToValue(content, value)){
                return k_status_ok;
            }else{
                *extra_info = name;
                return k_status_invalid;
            }
        }
    }
    return k_status_not_found;
}


static int parse_bool_argument_extra(const std::string& argument, std::string* extra_info){
    (void)(extra_info);

    static const std::vector<std::pair<const char*, bool>> prefixes = {{"-", true}, {"--", true}, {"-no", false}, {"--no", false}};

    for(CommandLineFlagRegistry<bool>::Command& command : CommandLineFlagRegistry<bool>::instance()->commands){
        if(argument.size() > command.name.size()){
            size_t diff = argument.size() - command.name.size() - 1UL;
            
            if(diff < prefixes.size()){
                const std::string& prefix = std::get<0>(prefixes[diff]);
                if(argument == prefix + command.name){
                    *command.value = std::get<1>(prefixes[diff]);
                    return k_status_ok;
                }
            }
        }
    }
    return k_status_not_found;
}

template<typename T>
static void print_type_usage(){
    for(auto& command : CommandLineFlagRegistry<T>::instance()->commands){
        std::string& name = command.name;
        name = "--" + name;
        std::string& desc = command.text;
        T& default_value = command.default_value;
        std::cerr << std::setw(20) << name << ": " << desc << "[default:" << default_value <<"]." << std::endl;
    }
}

template<typename ...TS>
static void print_type_usages(){
    int unused[] = {0, (print_type_usage<TS>(), 0) ...};
    (void)(unused);
}

static void print_usage_and_exit(const char* argv0){
    std::cerr << "Program " << argv0 << "Flags: " << std::endl;
    print_type_usages<bool, int32_t, std::string, double, int64_t, uint64_t>();
    exit(1);
}

static void print_parse_error(const std::string& name,
                              const char* actual_input,
                              const char* arg0){
    std::cerr << "parse command flag " << name <<" error. User input is " << actual_input << std::endl;
    print_usage_and_exit(arg0);
}

void parse_commandline_flags(int* argc,
                             char** argv,
                             bool with_help){
    int unused_argc = 1;
    std::string extra;

    for(int i = 1; i < *argc; i++){
        std::string arg = argv[i];
        int s = k_status_invalid;

#define ParseArgumentWithType(type) \
        s = parse_argument<type>(arg, &extra); \
        if(s == k_status_ok){ \
            continue; \
        }else if(s == k_status_invalid){ \
            print_parse_error(extra, argv[i],argv[0]);\
        }

        ParseArgumentWithType(bool);
        ParseArgumentWithType(int32_t);
        ParseArgumentWithType(double);
        ParseArgumentWithType(int64_t);
        ParseArgumentWithType(uint64_t);
        ParseArgumentWithType(std::string);
#undef ParseArgumentWithType

        s = parse_bool_argument_extra(arg, &extra);
        if(s == k_status_ok){
            continue;
        }

        if(with_help && (arg == "--help" || arg == "-h")){
            print_usage_and_exit(argv[0]);
        }

        std::swap(argv[unused_argc++], argv[i]);
    }
    *argc = unused_argc;
}

}//namespace utils
}//namespace ccml
