/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 15:09
* Last modified: 2016-10-18 15:09
* Filename: Allocator.h
* Description: 
**********************************************/


#pragma once

#include <mutex>
#include <stdlib.h>
#include "utils/Logging.h"

namespace ccml{
namespace math{

class Allocator{
public:
    virtual ~Allocator(){};
    virtual void* alloc(size_t size) = 0;
    virtual void free(void* ptr) = 0;
    virtual std::string get_name() = 0;
}; //class Allocator

class CpuAllocator : public Allocator{
public:
    ~CpuAllocator(){}

    virtual void* alloc(size_t size){
        void* ptr;
        CC_CHECK_EQ(posix_memalign(&ptr, 32ul, size), 0);
        CC_CHECK(ptr) << "Fail to allocate CPU memory: size=" << size;
        return ptr;
    }

    virtual void free(void* ptr){
        if(ptr){
            ::free(ptr);
        }
    }

    virtual std::string get_name(){
        return "cpu_alloc";
    }
};

}//namespace math
}//namespace ccml
