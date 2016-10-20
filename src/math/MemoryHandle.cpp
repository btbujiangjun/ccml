/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 13:57
* Last modified: 2016-10-18 13:57
* Filename: MemoryHandle.cpp
* Description: 
**********************************************/

#include "MemoryHandle.h"
#include <cmath>
#include "Storage.h"

namespace ccml{
namespace math{

/**
 * Calculate the actual allocation size according to the required size.
 */
MemoryHandle::MemoryHandle(size_t size):_size(size), _buf(nullptr){
    if(_size <= 256){
        _alloc_size = 256;
    }else if(_size <= 512){
        _alloc_size = 512;
    }else if(_size <= (1 << 16)){
        _alloc_size = (size + 1023) & ~(1023);//allocate multiple of 1024 bytes.
    }else{
        _alloc_size = _size;
    }
}


CpuMemoryHandle::CpuMemoryHandle(size_t size) : MemoryHandle(size){
    CC_CHECK(size != 0) << "allocate 0 bytes.";
    _allocator = StorageEngine::singleton()->get_cpu_allocator();
    _buf = _allocator->alloc(_alloc_size);
}

CpuMemoryHandle::~CpuMemoryHandle(){
    _allocator->free(_buf, _alloc_size);
}
}//namespace math
}//namespace ccml
