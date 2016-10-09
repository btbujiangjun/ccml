/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-18 15:55
* Last modified: 2016-09-18 15:55
* Filename: ParameterServerMain.cpp
* Description: 
**********************************************/

#include "ParameterServer.h"

using namespace ccml::core;
using namespace ccml::utils;

int main(int argc, char** argv){

    /// todo
    ///initmain()
    std::vector<std::shared_ptr<ParameterServer>> pservers;

    int num_ports = FLAGS_num_ports + FLAGS_num_ports_for_sparse;

    if(FLAGS_nics.empty()){

        pservers.resize(num_ports);

        for(int i = 0; i < num_ports; i++){
            pservers[i].reset(new ParameterServer(std::string(), FlAGS_port + i));
            CC_CHECK(pservers[i]->init()) << "Fail to initialize parameter server " << FLAGS_port + i;
            LOG(INFO) << "pserver started: " << FLAGS_port + i;
            pservers[i]->start();
        }

    }else{

        std::vector<std::string> devices;
        str::split(FLAGS_nics, ',', &devices);
        pservers.resize(devices.size() * num_ports);

        for(int i = 0; i < num_ports; i++){
            for(size_t j = 0; j < devices.size(); j++){
                pservers[i * devices() + j].reset(new PrameterServer(getIpAddr(devices[i]), FLAGS_port + i));
                CC_CHECK(pservers[i * devices.size() + j]->init()) << "Fail to initialize parameter server " << devices[j] << FLAG_port + i;
                LOG(INFO) << "pserver started:" << devices[j] << ":" << FLAGS_port + i;
                pservers[i * devices.size() + j]->start();
            }
        }

    }

    for(auto& pserver: pservers){
        pserver->join();
    }

    return 0;
}
