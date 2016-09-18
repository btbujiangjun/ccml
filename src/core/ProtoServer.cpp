/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-14 14:40
* Last modified:	2016-09-14 15:36
* Filename:		ProtoServer.cpp
* Description: 
**********************************************/

#include "ProtoServer.h"
#include <sys/time.h>

namespace ccml{
namespace core{

void ProtoServer::handle_request(std::unique_ptr<MessageReader> message_reader, ResponseCallback callback){

    /// 0 for funcation name;
    /// 1 for proto
    CC_CHECK_GE(message_reader->get_num_blocks(), (size_t)2);
    std::string function_name(message_reader->get_next_block_length(), 0);

    /// read function name string
    message_reader->read_next_block(&function_name[0]);
    /// looking up rpc wrapped callback function
    auto it = _name_to_function_map.find(function_name);
    if(it != _name_to_function_map.end()){
#ifndef CCML_DISABLE_TIMER
        gettimeofday(&(*(_handle_request_begin)), nullptr);
#endif
        it->second(std::move(message_reader), callback);
    }else{
        LOG(ERROR) << "Unknown function_name: " << function_name;
        std::vector<iovec> iovs;
        callback(iovs);
    }
}

void ProtoServer::register_service_function_implement(const std::string& function_name, ServiceFunction function){
    CC_CHECK(!_name_to_function_map.count(function_name)) << "Duplicated registration: " << function_name;
    _name_to_function_map[function_name] = function;
}


void ProtoClient::send(const char* function_name,
                       const google::protobuf::MessageLite& proto,
                       const std::vector<iovec>& user_iov){
    std::string proto_string;
    CC_CHECK(proto.SerializeToString(&proto_string));
    std::vector<iovec> iovs;
    iovs.reserve(iovs.size() + 2);

    ///sending function name string, protobuf data and user addtional data
    iovs.push_back({(void*)function_name,strlen(function_name)});
    iovs.push_back({&proto_string[0], proto_string.size()});
    iovs.insert(iovs.end(), user_iov.begin(), user_iov.end());
    _channel->write_message(iovs);
}

std::unique_ptr<MessageReader> ProtoClient::recv(google::protobuf::MessageLite* proto){
    std::vector<iovec> iovs;
    
    std::unique_ptr<MessageReader> message_reader = _channel->read_message();
    CC_CHECK_GE(message_reader->get_num_blocks(), (size_t)1);
    std::string str(message_reader->get_next_block_length(), 0);
    message_reader->read_next_block(&str[0]);

    CC_CHECK(proto->ParseFromString(str));
    
    return message_reader;
}

}//namespace core
}//namespace ccml
