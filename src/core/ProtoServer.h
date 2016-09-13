/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-13 16:09
* Last modified: 2016-09-13 16:09
* Filename: ProtoServer.h
* Description: 
**********************************************/

#ifndef CCML_CORE_PROTOSERVER_H
#define CCML_CORE_PROTOSERVER_H

#include "LightServer.h"
#include <google/protobuf/message_lite.h>

namespace ccml{
namespace core{
class ProtoServer : public SocketSever{
public:
    ProtoServer(const std::string& addr, int port) : SocketServer(addr, port){
    }

    typedef std::function<void(const google::protobuf::MessageLite& proto_out)> ProtoResponseCallback;
    typedef std::function<void(const google::protobuf::MessageList& proto_out, const std::vector<iovec>& output_iovs)> ProtoResponseCallbackException;

    template <class T>
    void register_service_function_exception(\
            const std::string& function_name, \
            std::function<void(const T&, std::unique_ptr<MessageReader> messager_reader, ProtoResponseCallbackException callback)> function);

protected:
    virtual void handle_request(std::unique_ptr<MessageReader> message_reader, ResponseCallback callback);

    typedef std::function<void(std::unique_ptr<MessageReader> message_reader, ResponseCallback callback)> ServiceFunction;

    void register_service_function_implement(const std::string& function_name, ServiceFunction function);


    ThreadLocal<struct timeval> _handle_request_begin;

    std::map<std::string, ServiceFunction> _name_to_function_map;

};//class SocketServer


class ProtoClient : public SocketClient{
public:
    ProtoClient(const std::string& server_addr, int server_port) : SocketClient(server_addr, server_port){
    }

    void send(const char* function_name, const google::protobuf::MessageLite& proto, const std::vector<iovec>& ivo = std::vector<iovec>());

    std::unique_ptr<MessageReader> recv(google::protobuff::MessageLite* proto);

    std::unique_ptr<MessageReader> send_recv(const char* function_name, const google::protobuf::MessageLite& proto_in, google::protobuf::MessageLite* proto_out){
        send(function_name, proto_in);
        return recv(proto_out);
    }

    std::unique_ptr<MessageReader> send_recv(const char* function_name, const google::protobuf::MessageLite& proto_in, const std::vector<iovec>& iovs, google::protobuf::MessageLite* proto_out){
        send(function_name, proto_in, iovs);
        return recv(proto_out);
    }
};//class ProtoClient




}//namespace core
}//namespace ccml

#endif
