/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-18 19:47
* Last modified: 2016-09-18 19:47
* Filename: ParameterServer.h
* Description: 
**********************************************/

#ifndef CCML_CORE_PARAMETERSERVER_H
#define CCML_CORE_PARAMETERSERVER_H

#include "ProtoServer.h"
#include "Locks.h"
#include <unordered_map>


namespace ccml{
namespace core{
class ParameterServer : public ProtoServer{
public:

protected:
    RWLock _parameter_mutex;

    typedef std::pair<size_t, int64_t> BlockKey;
    struct BlockKeyHash{
        size_t operator()(const BlockKey& key) const{
            return std::hash<size_t>()(key.first) + key.second;
        }
    };

    /// param1:key_type
    /// param2:mapped_type
    /// param3:hasher
    typedef std::unordered_map<BlockKey, int64_t, BlockKeyHash> BlockMap;
    /// global idx
    BlockMap _block_id_map;
    /// golobal offset(byte) in all parameters
    BlockMap _block_offset_map;

    std::vector<CpuVectorPtr> _cpu_vectors;
    std::vector<CpuMatrixPtr> _cpu_matrixes;
    std::vector<CpuMemHandlePtr> _cpu_data_mems;

};//class ParameterServer
}//namespace core
}//namespace ccml

#endif
