/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-13 16:09
* Last modified:	2016-09-14 14:40
* Filename:		ProtoServer.h
* Description: 
**********************************************/

#ifndef CCML_CORE_PROTOSERVER_H
#define CCML_CORE_PROTOSERVER_H

#include <map>
#include "LightServer.h"
#include "ThreadLocal.h"
#include "SocketChannel.h"
#include <google/protobuf/message_lite.h>

namespace ccml{
namespace core{
class ProtoServer : public SocketServer{
public:
    ProtoServer(const std::string& addr, int port) : SocketServer(addr, port){}

    typedef std::function<void(const google::protobuf::MessageLite& proto_out)> ProtoResponseCallback;

    template <class T>
    void register_service_function(const std::string& function_name,
                                   std::function<void(const T& request, ProtoResponseCallback callback)> function);

    typedef std::function<void(const google::protobuf::MessageLite& proto_out, const std::vector<iovec>& output_iovs)> ProtoResponseCallbackEx;

    template <class T>
    void register_service_function_ex(const std::string& function_name,
                                      std::function<void(const T&, std::unique_ptr<MessageReader> messager_reader,
                                      ProtoResponseCallbackEx callback)> function);

protected:
    virtual void handle_request(std::unique_ptr<MessageReader> message_reader, ResponseCallback callback);

    typedef std::function<void(std::unique_ptr<MessageReader> message_reader, ResponseCallback callback)> ServiceFunction;

    void register_service_function_implement(const std::string& function_name, ServiceFunction function);


    ThreadLocal<struct timeval> _handle_request_begin;

    std::map<std::string, ServiceFunction> _name_to_function_map;

};//class SocketServer


class ProtoClient : public SocketClient{
public:
    ProtoClient(const std::string& server_addr, int server_port) : SocketClient(server_addr, server_port){}

    void send(const char* function_name,
              const google::protobuf::MessageLite& proto,
              const std::vector<iovec>& ivo = std::vector<iovec>());

    std::unique_ptr<MessageReader> recv(google::protobuf::MessageLite* proto);

    std::unique_ptr<MessageReader> send_recv(const char* function_name,
                                             const google::protobuf::MessageLite& proto_in,
                                             google::protobuf::MessageLite* proto_out){
        send(function_name, proto_in);

        return recv(proto_out);
    }

    std::unique_ptr<MessageReader> send_recv(const char* function_name,
                                             const google::protobuf::MessageLite& proto_in,
                                             const std::vector<iovec>& iovs,
                                             google::protobuf::MessageLite* proto_out){
        send(function_name, proto_in, iovs);

        return recv(proto_out);
    }
};//class ProtoClient


template<class T>
void ProtoServer::register_service_function_ex(const std::string& function_name,
                                               std::function<void(const T&, std::unique_ptr<MessageReader> message_reader,
                                               ProtoResponseCallbackEx callback)> func){
    auto f = [func](std::unique_ptr<MessageReader> message_reader, ResponseCallback callback){
        T request;
        std::string str(message_reader->get_next_block_length(), 0);
        message_reader->read_next_block(&str[0]);
        CC_CHECK(request.ParseFromString(str));
        auto pcob = [callback](const google::protobuf::MessageLite& response, const std::vector<iovec>& output_iovs){
            std::string out;
            CC_CHECK(response.SerializeToString(&out));
            std::vector<iovec> iovs;
            iovs.push_back({&out[0], out.size()});
            iovs.insert(iovs.end(), output_iovs.begin(), output_iovs.end());
            callback(iovs);
        };
        func(request, std::move(message_reader), pcob);
    };
    register_service_function_implement(function_name, f);
}

template <class T>
void ProtoServer::register_service_function(const std::string& function_name,
                                            std::function<void(const T&, ProtoResponseCallback callback)> func){
    auto f = [func](std::unique_ptr<MessageReader> message_reader, ResponseCallback callback){
        T request;
        std::string str(message_reader->get_next_block_length(), 0);
        message_reader->read_next_block(&str[0]);
        CC_CHECK(request.ParseFromString(str));
        message_reader.reset();

        auto pcob = [callback](const google::protobuf::MessageLite& response){
            std::string out;
            CC_CHECK(response.SerializeToString(&out));
            std::vector<iovec> iovs;
            iovs.push_back({&out[0], out.size()});
            callback(iovs);
        };

        func(request, pcob);
    };
    register_service_function_implement(function_name, f);
}

}//namespace core
}//namespace ccml

#endif
