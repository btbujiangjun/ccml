/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-10 11:17
* Last modified: 2016-10-10 11:17
* Filename: util.cpp
* Description: 
**********************************************/

#include "Util.h"
#include "Logging.h"
#include "Version.h"

namespace ccml{
namespace utils{

typedef std::pair<int, std::function<void()>> PriorityFuncPair;
typedef std::vector<PriorityFuncPair> InitFuncList;

static bool g_initialized = false;
static InitFuncList* g_init_funcs = nullptr;
static std::once_flag  g_once_flag;

void register_init_function(std::funciton<void()>, func, int priority){
    if(g_initialized){
        LOG(FATAL) << "register_init_function() should only called before init_main().";
    }
    if(!g_initfuncs){
        g_initfuncs = new InitFuncList();
    }
    g_initfuncs->push_back(std::make_pair(priority, func));
}


void run_init_functions(){
    std::call_once(g_once_flag, [] (){
        LOG(INFO) << "calling run_init_functions.";
        if(g_init_funcs){

            std::sort(g_init_funcs->begin(), g_init_funcs->end(), [](const PriorityFuncPair& x, const PriorityFuncPair& y){
                return x.first > y.first;
            });

            for(auto& f : *g_init_funcs){
                f.second();
            }

            delete g_init_funcs;
            g_init_funcs = nullptr;
        }

        g_initialized = true;
        LOG(INFO) << "call run_init_functions done.";
    });
}

void init_main(int argc, char** argv){

    CC_CHECK_EQ(argc, 1) << "Unkown commandline argument:" << argv[0];

    initialize_logging(argc, argv);
    // install_layer_stack_tracer();

    std::string line;
    for(int i = 0; i < argc; i++){
        line += argv[i];
        line += ' ';
    }
    LOG(INFO) << "commandline: " << line;

    //parse_commandline_flags(&argc, argv, true);
    //install_profile_switch();

    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    if(FLAGS_seed == 0){
        unsigned int seed = time(NULL);
        srand(seed);
        ThreadLocalRand::init_seed(seed);
        LOG(INFO) << "random number seed = " << seed;
    }else{
        srand(FLAGS_seed);
        ThreadLocalRand::init_seed(FLAGS_seed);
    }

    version::print_version();
    run_init_functions();
}

}//namespace utils
}//namespace ccml
