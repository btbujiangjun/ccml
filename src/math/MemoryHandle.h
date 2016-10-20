/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 13:50
* Last modified: 2016-10-18 13:50
* Filename: MemoryHandle.h
* Description: 
**********************************************/

#pragma once

#include "PoolAllocator.h"
#include <memory>

namespace ccml{
namespace math{
class MemoryHandle{
protected:
    explicit MemoryHandle(size_t size);
    virtual ~MemoryHandle(){}

public:
    void* get_buf() const {
        return _buf;
    }

    size_t get_size() const{
        return _size;
    }

    size_t get_alloc_size() const{
        return _alloc_size;
    }

protected:
    PoolAllocator* _allocator;
    size_t _size; //the request size
    size_t _alloc_size; // the allocated size;
    void* _buf;
};//class MemoryHandle


class CpuMemoryHandle : public MemoryHandle{
public:
    explicit CpuMemoryHandle(size_t size);
    virtual ~CpuMemoryHandle();
}; //class CpuMemoryHandle

typedef std::shared_ptr<MemoryHandle> MemoryHandlePtr;
typedef std::shared_ptr<CpuMemoryHandle> CpuMemoryHandlePtr;

}//namespace math
}//namespace ccml
