/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-06 20:55
* Last modified: 2016-09-06 20:55
* Filename: SocketChannel.h
* Description: 
**********************************************/

#ifndef CCML_CORE_SOCKETCHANNEL_H
#define CCML_CORE_SOCKETCHANNEL_H

namespace ccml{
namespace core{
class SocketChannel{
public:

    SocketChannel(int socket, const std::string& peer_name) : _tcp_socket(socket), _peer_name(peer_name){
    }
    ~SocketChannel();

    const std::string& get_peer_name()const{
        return _peer_name;
    }

    size_t read(void* buf, size_t size);
    size_t write(const void* buf, size_t size);

    size_t readv(std::vector<struct iovec>* iov);
    size_t writev(const std::vector<struct iovec>& iov);

    void write_message(const std::vector<struct iovec>& iov);
    std::unique_ptr<MessageReader> read_message();

protected:

    int _tcp_socket;
    const std::string _peer_name;

    struct MessageHeader{
        int64_t total_length;
        int64_t num_iovs;
        int64_t iov_lengths[0];
    }
};//class SocketChannel

class MessageReader{
public:
    MessageReader(SocketChannel* channel, size_t num_iovs);
    ~MessageReader(){
    }
};//class MessageReader

}//namespace core
}//namespace ccml
#endif
