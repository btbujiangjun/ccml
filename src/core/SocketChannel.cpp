/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-08 16:10
* Last modified:	2016-09-09 17:39
* Filename:		SocketChannel.cpp
* Description: 
**********************************************/

#include "SocketChannel.h"
#include "../utils/Logging.h"
#include <unistd.h>

namespace ccml{
namespace core{

SocketChannel::~SocketChannel(){
    close(_tcp_socket);
    LOG(INFO) << "destory connect in socket channel, peer = " << _peer_name;
}

size_t SocketChannel::read(void* buf, size_t size){
    size_t total = 0;
    while(total < size){
        ssize_t length = ::read(_tcp_socket, (char*)buf+total, size-total);
        CC_CHECK(length >= 0) << " peer = " << _peer_name;
        if(length <= 0){
            return total;
        }
        total += length;
    }
    return total;
}

size_t SocketChannel::write(const void* buf, size_t size){
    size_t total = 0;
    while(total < size){
        ssize_t length = ::write(_tcp_socket, (const char*)buf + total, size - total);
        CC_CHECK(length >= 0) << " peer = " << _peer_name;
        if(length <= 0){
            return total;
        }
        total += length;
    }
    return total;
}

template<class IOFUNC, class SOCKET>
static size_t readwritev(IOFUNC io_func, SOCKET socket, iovec* iovs, int num_iovs, int max_iovs, const std::string& peer_name){
    int current_iov = 0;
    size_t total = 0;

    for(int i = 0; i < num_iovs; i++){
        total += iovs[i].iov_len;
    }

    size_t size = 0;
    size_t current_iov_size_done = 0;

    while(size < total){
        ssize_t length = io_func(socket, &iovs[current_iov], std::min(num_iovs- current_iov, max_iovs));
        CC_CHECK(length > 0) << " peer = " << peer_name << " current_iov = " << current_iov << " num_iov = " << num_iovs << " iovs[current_iov].base = " << iovs[current_iov].iov_base << " iovs[current_iov].iov_len = " << iovs[current_iov].iov_len;
        size += length;

        iovs[current_iov].iov_base = (void*)((char*)iovs[current_iov].iov_base - current_iov_size_done);

        iovs[current_iov].iov_len += current_iov_size_done;
        length += current_iov_size_done;

        while(current_iov < num_iovs){
            if((size_t)length < iovs[current_iov].iov_len){
                break;
            }
            length -= iovs[current_iov].iov_len;
            ++current_iov;
        }
        if(current_iov < num_iovs){
            current_iov_size_done = length;
            iovs[current_iov].iov_base = (void*)((char*)iovs[current_iov].iov_base + length);
            iovs[current_iov].iov_len -= length;
        }
    }
    return size;
}

size_t SocketChannel::readv(std::vector<struct iovec>* iovs){
    return readwritev(::readv, _tcp_socket, const_cast<iovec*>(&(*iovs)[0]), iovs->size(), UIO_MAXIOV, _peer_name);
}

size_t SocketChannel::writev(const std::vector<struct iovec>& iovs){
    return readwritev(::writev, _tcp_socket, const_cast<iovec*>(&iovs[0]),iovs.size(), UIO_MAXIOV, _peer_name);
}

std::unique_ptr<MessageReader> SocketChannel::read_message(){
    MessageHeader header;
    size_t length = read(&header,sizeof(header));
    if(length == 0){
        return nullptr;
    }

    CC_CHECK(length == sizeof(header));

    std::unique_ptr<MessageReader> message_reader(new MessageReader(this, header.num_iovs));
    CC_CHECK_EQ(message_reader->get_total_length() + sizeof(header) + message_reader->get_num_blocks() * sizeof(size_t), (size_t)header.total_length) << " total_length = " << message_reader->get_total_length() << " num_blocks = " << message_reader->get_num_blocks();
    return message_reader;
}

void SocketChannel::write_message(const std::vector<struct iovec>& user_iovs){
    MessageHeader header;
    header.num_iovs = user_iovs.size();

    std::vector<size_t> iov_lengths;
    iov_lengths.reserve(user_iovs.size());
    for(auto& iov : user_iovs){
        iov_lengths.push_back(iov.iov_len);
    }

    std::vector<iovec> iovs;
    iovs.reserve(user_iovs.size()+2);
    iovs.push_back({&header, sizeof(header)});
    iovs.push_back({&iov_lengths[0], sizeof(iov_lengths[0]) * (size_t)header.num_iovs});
    iovs.insert(iovs.end(), user_iovs.begin(), user_iovs.end());

    header.total_length = 0;
    for(auto& iov : iovs){
        header.total_length += iov.iov_len;
    }

    CC_CHECK(writev(iovs) == (size_t)header.total_length);
}//end SocketChannel


MessageReader::MessageReader(SocketChannel* channel, size_t num_blocks) : _channel(channel), _num_blocks(num_blocks), _current_block_index(0) {
    size_t size = num_blocks * sizeof(_num_blocks[0]);
    CC_CHECK(_channel->read(&_num_blocks[0], size) == size);
}

void MessageReader::read_blocks(const std::vector<void*>& bufs){
    CC_CHECK_LE(_current_block_index + bufs.size(), _num_blocks.size());

    std::vector<iovec> iovs;
    iovs.reserve(bufs.size());
    size_t total = 0;
    for(void* buf :bufs){
        iovs.push_back({buf, get_next_block_length()});
        total += get_next_block_length();
        ++_current_block_index;
    }

    CC_CHECK(_channel->readv(&iovs) == total);
}

void MessageReader::read_next_block(void* buf){
    CC_CHECK_LT(_current_block_index, _num_blocks.size());
    CC_CHECK(_channel->read(buf, get_next_block_length()) == get_next_block_length());
    ++_current_block_index;
}


}//namespace core
}//namespace ccml
