/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-18 15:36
* Last modified: 2016-10-18 15:36
* Filename: Storage.cpp
* Description: 
**********************************************/

#include "Storage.h"
#include "Allocator.h"
#include "utils/Util.h"

CC_DEFINE_int32(pool_limit_size, 536870912, "maximum memory size managed by a memory pool, default is 512M");

namespace ccml{
namespace math{

static ccml::utils::InitFunction __ini_storage_engine([](){StorageEngine::singleton();}, std::numeric_limits<int>::max());

StorageEngine::StorageEngine() : _cpu_allocator(nullptr){}

StorageEngine::~StorageEngine(){
    delete _cpu_allocator;
}

StorageEngine* StorageEngine::singleton(){
    static StorageEngine storage;
    return &storage;
}

PoolAllocator* StorageEngine::get_cpu_allocator(){
    {
        ccml::core::ReadLockGuard guard(_lock);
        if(_cpu_allocator != nullptr){
            return _cpu_allocator;
        }
    }
    {
        std::lock_guard<ccml::core::RWLock> guard(_lock);
        if(_cpu_allocator == nullptr){
            _cpu_allocator = new PoolAllocator(new CpuAllocator(), FLAGS_pool_limit_size, "cpu_pool");
        }
        return _cpu_allocator;
    }
}

}//namespace math
}//namespace ccml
