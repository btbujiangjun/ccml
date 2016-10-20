/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 15:19
* Last modified: 2016-10-18 15:19
* Filename: Storage.h
* Description: 
**********************************************/

#pragma once

#include <mutex>
#include <vector>
#include "core/Locks.h"
#include "utils/CommandLineParser.h"
#include "PoolAllocator.h"

namespace ccml{
namespace math{

class StorageEngine{
public:
    static StorageEngine* singleton();

    PoolAllocator* get_cpu_allocator();

protected:
    StorageEngine();
    ~StorageEngine();

    ccml::core::RWLock _lock;
    PoolAllocator* _cpu_allocator;

};//class StorageEngine

}//namespace math
}//namespace ccml
