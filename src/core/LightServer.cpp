/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-07 16:33
* Last modified:	2016-09-13 16:09
* Filename:		LightServer.cpp
* Description: 
**********************************************/

#include "LightServer.h"
#include "utils/Logging.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

namespace ccml{
namespace core{

void SocketWorker::run(){
    LOG(INFO) << "worker started, peer = " << _channel->get_peer_name();

    std::vector<iovec> input_iovs;

    while(true){
        std::unique_ptr<MessageReader> message_reader = _channel->read_message();
        if(!message_reader){
            break;
        }

        auto callback = [this](const std::vector<iovec>& output_iovs){
            _channel->write_message(output_iovs);
        };

        _server->handle_request(std::move(message_reader), callback);
    }

    LOG(INFO) << "worker begin to finish, peer = " << _channel->get_peer_name();
}

SocketServer::SocketServer(const std::string &addr, int port) :
    _addr(addr), _port(port){
    _stopping = false;
    _socket = 0;
    _max_pending_connections = 100;
}

SocketServer::~SocketServer(){
    _stopping = true;
    {
        SocketClient trigger(_addr.empty() ? "127.0.0.1": _addr, _port);
    }
    this->join();
}

void SocketServer::tcp_server(){
    int newsockfd;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    struct hostent* server;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    CC_CHECK(_socket >= 0) << "ERROR opening socket";

    memset((char*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    if(!_addr.empty()){
        server = gethostbyname(_addr.c_str());
        CC_CHECK(server) << "ERROR, no such host: " << _addr;
        bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
    }else{
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }

    set_option(_socket);

    CC_CHECK(bind(_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) >= 0) << \
        "ERROR on binding " <<_addr;

    listen(_socket, _max_pending_connections);
    client_len = sizeof(client_addr);

    while(true){
        newsockfd = accept(_socket, (struct sockaddr*)&client_addr, &client_len);
        if(_stopping){
            break;
        }
        CC_CHECK(newsockfd >=0 ) << "ERROR on accept.";
        constexpr int k_peer_name_len = 128;
        char peer_name[k_peer_name_len];
        CC_CHECK(inet_ntop(AF_INET, &client_addr.sin_addr, peer_name, k_peer_name_len));

        SocketWorker* worker = new SocketWorker(create_channel(newsockfd, std::string(peer_name)), this);
        worker->start();
        worker->detach();
    }
    close(_socket);
    LOG(INFO) << "LightServer accept thread finished, addr=" << _addr << " port=" << _port;
}

void SocketServer::run(){
    LOG(INFO) << "tcp server started.";
    tcp_server();
}


SocketClient::SocketClient(const std::string &server_addr, int server_port){
    struct sockaddr_in serv_addr;
    struct hostent hostinfo, *server;
    char buf[1024];
    int error_return;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    CC_CHECK(sockfd >= 0) << "ERROR opening socket.";
    CC_CHECK_EQ(0, gethostbyname_r(server_addr.c_str(), &hostinfo, buf, sizeof(buf), &server, &error_return)) << \
        "ERROR, no such host: " << server_addr << " ret= " << error_return;
    CC_CHECK(server) << "gethostbyname_r error.";

    memset((char*)&server_addr, 0, sizeof(server_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(server_port);

    set_option(sockfd);

    CC_CHECK(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0) << \
        "ERROR connecting to " << server_addr;
    _channel.reset(new SocketChannel(sockfd, server_addr));
}


void set_option(int sockfd){
    int send_size = 1024 * 1024 * 40;
    int recv_size = 1024 * 1024 * 40;
    bool FLAGS_small_messages = false;

    CC_CHECK_GE(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv_size, sizeof(recv_size)), 0);
    CC_CHECK_GE(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv_size, sizeof(recv_size)), 0);
    CC_CHECK_GE(setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &send_size, sizeof(send_size)), 0);

    if(FLAGS_small_messages){
        int optval = 1;
        CC_CHECK_GE(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)), 0);
        optval = 1;
        CC_CHECK_GE(setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &optval, sizeof(optval)), 0);
    }
    int reuse = 1;
    CC_CHECK_GE(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)), 0);
}

std::string get_ip_addr(std::string &device){
    int sockfd;
    struct sockaddr_in sin;
    struct ifreq ifr;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    CC_CHECK(sockfd >= 0) << "Create socket error.";

    strncpy(ifr.ifr_name, device.c_str(), IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    CC_CHECK_GE(ioctl(sockfd, SIOCGIFADDR, &ifr), 0);
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    close(sockfd);

    return std::string(inet_ntoa(sin.sin_addr));
}

}//namespace core
}//namesapce ccml
