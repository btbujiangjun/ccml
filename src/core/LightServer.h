/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-07 15:56
* Last modified:	2016-09-09 18:54
* Filename:		LightServer.h
* Description: 
**********************************************/


#ifndef CCML_CORE_LIGHTSERVER_H
#define CCML_CORE_LIGHTSERVER_H

#include "Thread.h"
#include "SocketChannel.h"
#include <vector>
#include <sys/uio.h>

namespace ccml{
namespace core{

class SocketWorker;

class SocketServer : public Thread {
public:
    SocketServer(const std::string& addr, int port);
    ~SocketServer();

    virtual void run();

    typedef std::function<void(const std::vector<iovec>& output_iovs)> ResponseCallback;

protected:
    virtual void handle_request(std::unique_ptr<MessageReader> message_reader,
            ResponseCallback callback) = 0;

    std::unique_ptr<SocketChannel> create_channel(int sock, const std::string& peer_name){
        return std::unique_ptr<SocketChannel>(new SocketChannel(sock, peer_name));
    }


    friend class SocketWorker;
    int _socket;
    int _port;
    std::string _addr;
    int _max_pending_connections;
    bool _stopping;

private:
    void tcp_server();
    void detach(){}
};//class SocketServer

class SocketClient{
public:
    SocketClient(const std::string& server_addr, int server_port);
    SocketChannel* get_socketchannel(){
        return _channel.get();
    }

protected:
    std::unique_ptr<SocketChannel> _channel;

};//class SocketClient

class SocketWorker : public Thread {
public:
    SocketWorker(std::unique_ptr<SocketChannel>&& channel, SocketServer* server) :
        _channel(std::move(channel)), _server(server){
    }

    virtual ~SocketWorker(){}

    virtual void run();

protected:
    std::unique_ptr<SocketChannel> _channel;
    SocketServer* _server;
};//class SocketWorker

void set_option(int sockfd);
std::string get_ip_addr(std::string& device);

}//namespace core
}//namespace ccml
#endif
