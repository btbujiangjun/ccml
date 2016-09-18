/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-18 15:55
* Last modified: 2016-09-18 15:55
* Filename: ParameterServerMain.cpp
* Description: 
**********************************************/


using namespace ccml::core;
using namespace ccml::utils;

int main(int argc, char** argv){
    /// todo
    ///initmain()
    std::vector<std::string> devices;
    std::vector<std::shared_ptr<ParameterServer>> pservers;



    for(auto& pserver: pservers){
        pserver->join();
    }

    return 0;
}
