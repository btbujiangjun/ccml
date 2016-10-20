/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 14:29
* Last modified: 2016-10-18 14:29
* Filename: PoolAllocator.cpp
* Description: 
**********************************************/

#include "PoolAllocator.h"

namespace ccml{
namespace math{

PoolAllocator::PoolAllocator(Allocator* allocator,
                             size_t size_limit,
                             const std::string& name) :
                             _allocator(allocator),
                             _size_limit(size_limit),
                             _pool_memory_size(0),
                             _name(name){}

PoolAllocator::~PoolAllocator(){
    free_all();
}

void* PoolAllocator::alloc(size_t size){
    if(_size_limit > 0){
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _pool.find(size);
        if(it == _pool.end() || it->second.size() == 0){
            if(_pool_memory_size >= _size_limit){
                free_all();
            }
            return _allocator->alloc(size);
        }else{
            auto buf = it->second.back();
            it->second.pop_back();
            _pool_memory_size = size;
            return buf;
        }
    }else{
        return _allocator->alloc(size);
    }
}

void PoolAllocator::free(void* ptr, size_t size){
    if(_size_limit > 0){
        std::lock_guard<std::mutex> guard(_mutex);
        auto& it = _pool[size];
        it.push_back(ptr);
        _pool_memory_size += size;
    }else{
       _allocator->free(ptr);
    }
}

void PoolAllocator::free_all(){
    for(auto it : _pool){
        for(auto ptr : it.second){
            _allocator->free(ptr);
        }
    }
    _pool_memory_size = 0;
    _pool.clear();
}

void PoolAllocator::print_all(){
    size_t memory_size = 0;
    LOG(INFO) << _name << ":";
    for(auto it : _pool){
        LOG(INFO) << " size:" << it.first;
        for(auto ptr : it.second){
            LOG(INFO) << "  ptr:" << ptr;
            memory_size += it.first;
        }
    }
    LOG(INFO) << "memory size: " << memory_size;
}


}//namespace match
}//namespace ccml
