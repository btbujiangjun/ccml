/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 14:19
* Last modified: 2016-10-18 14:19
* Filename: PoolAllocator.h
* Description: 
**********************************************/

#pragma once

#include "Allocator.h"
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <map>

namespace ccml{
namespace math{

class PoolAllocator{
public:
    PoolAllocator(Allocator* allocator,
                  size_t size_limit =0,
                  const std::string& name = "pool");

    ~PoolAllocator();

    void* alloc(size_t size);
    void free(void* ptr, size_t size);
    std::string get_name(){
        return _name;
    }

private:
    void free_all();
    void print_all();
    std::unique_ptr<Allocator> _allocator;
    std::mutex _mutex;
    std::unordered_map<size_t, std::vector<void*>> _pool;
    size_t _size_limit;
    size_t _pool_memory_size;
    std::string _name;
};//class PoolAllocator

}//namespace math
}//namespace ccml
