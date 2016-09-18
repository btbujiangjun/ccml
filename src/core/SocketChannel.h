/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-06 20:55
* Last modified: 2016-09-06 20:55
* Filename: SocketChannel.h
* Description: 
**********************************************/

#ifndef CCML_CORE_SOCKETCHANNEL_H
#define CCML_CORE_SOCKETCHANNEL_H

#include "../utils/Logging.h"
#include <memory>
#include <vector>
#include <sys/uio.h>

namespace ccml{
namespace core{

class MessageReader;

class SocketChannel{
public:

    SocketChannel(int socket, const std::string& peer_name) : 
        _tcp_socket(socket), _peer_name(peer_name){}

    ~SocketChannel();

    const std::string& get_peer_name() const{
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
    };
};//class SocketChannel

class MessageReader{
public:
    MessageReader(SocketChannel* channel, size_t num_iovs);

    ~MessageReader(){
        CC_CHECK_EQ(_current_block_index, _num_blocks.size());
    }

    size_t get_num_blocks() const{
        return _num_blocks.size() - _current_block_index;
    }
    size_t get_next_block_length() const{
        return get_block_length(0);
    }
    size_t get_total_length() const{
        size_t total = 0;
        for (size_t i = _current_block_index; i < _num_blocks.size(); i++){
            total += _num_blocks[i];
        }
        return total;
    }
    size_t get_block_length(size_t i) const{
        return _num_blocks[_current_block_index + 1];
    }

    void read_blocks(const std::vector<void*>& bufs);
    void read_next_block(void* buf);

protected:
    SocketChannel* _channel;
    std::vector<size_t> _num_blocks;
    size_t _current_block_index;
};//class MessageReader

}//namespace core
}//namespace ccml
#endif
