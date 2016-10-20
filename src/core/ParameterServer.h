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
#include "math/Vector.h"
#include <unordered_map>


namespace ccml{
namespace core{
class ParameterServer : public ProtoServer{
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

    std::unordered_map<size_t, ParameterConfig> _config_map;

    struct BlockInfo{
        const ParameterConfig* config;
        std::unique_ptr<std::mutex> lock;
        uint64_t offset;
        std::unique_ptr<ParameterOptimizer> optimizer;
    };

    std::vector<BlockInfo> _block_infos;

    typedef std::vector<std::pair<int64_t, int64_t>> BlockSegments;
    ///keep a record of which segments are used.
    BlockSegments _used_segments;
    ///record pserver status
    PServerStatus _status;

    std::atomic<int64_t> _num_samples_processed;

    double _cost;
    int _mpi_size;
    int _data_size;

    OptimizationConfig _config;


    template<typename T, size_t AlignBytes>
    class ReadWriteBuffer : public std::vector<T, AlignedAllocator<T, AlignBytes>>{
    public:
        static_assert(sizeof(T) % AlignBytes == 0 || AlignBytes % sizeof(T) == 0, "Type T must be able to aligned.");
        constexpr static bool IsTLargerThanAlign = sizeof(T) >= AlignBytes;

        static_assert(std::is_pod<T>::value, "T must be POD type.");

        constexpr static size_t AlignElementCount = AlignBytes / sizeof(T);

        static_assert(AlignElementCount == (AlignElementCount & -AlignElementCount) || AlignBytes > sizeof(T), "AlignElementCount should be exp of 2.");



        void resize_with_align_hints(size_t size, size_t align_block_count =1){
            if(IsTLargerThanAlign){
                this->resize(size);
            }else{
                this->resize(size + align_block_count * (AlignElementCount - 1));
            }
        }
        
        void reset_align_alloc(){
            this->_current_offset = 0;
        }

        T* next_block(size_t block_size){
            T* r = &this->operator[](_current_offset);
            _current_offset += block_size;

            if(!IsTLargerThanAlign){
                _current_offset = (_current_offset + AlignElementCount - 1) & ~(AlignElementCount - 1);
            }

            return r;
        }

    private:
        size_t _current_offset;
    };//class ReadWriteBuffer


    ThreadLocal<ReadWriteBuffer<real, ALIGN_HINT>> _read_write_buffer;

    int64_t _size;//size of the parameter

    ThreadBarrier _gradient_ready_barrier;
    ThreadBarrier _parameter_ready_barrier;
    ThreadBarrier _pass_barrier;
    ThreadLocal<std::vector<SendParameterRequest>> _request_vec;
    ThreadLocal<std::vector<ProtoResponseCallbackEx>> _callback_vec;

    std::atomic<int> _num_pass_finish_clients;
    bool _all_client_pass_finish;

    std::vector<std::unique_ptr<ThreadBarrier>> _synchronize_barriers;
    std::atomic<int> _server_id;

    int64_t _async_lagged_threshold;
    std::atomic<int64_t> _async_update_steps;
    std::vector<int64_t> _async_trainer_steps;
    size_t _async_lagged_gradients_num;
    std::vector<size_t> _async_update_stat;
    std::vector<size_t> _async_trainer_discard_stat;
    std::vector<size_t> _async_trainer_commit_stat;

    std::unique_ptr<SyncThreadPool> _sync_threadpool;
    bool _is_sparse_server;
    std::atomic<int64_t> _batch_id;

    ThreadLocal<struct timeval> _add_grad_begin;

    std::unique_ptr<StatSet> _stat_set;

public:
    struct Buffer{
        real* base;
        size_t size;
    };



protected:
    bool async_gradient_commit_check_and_stat(const SendParameterRequest& request);
    void print_async_gradient_commit_stat_and_reset();


public:
    ParameterServer(const std::string& addr, int port);
    ~ParameterServer(){}

    static const std::string k_retmsg_invalid_matrix_handle;
    static const std::string k_retmsg_invalid_vector_handle;
    static const std::string k_retmsg_unknown_operation;

    template<typename Dtype>
    void reduce_and_send_data(const SendDataRequest& request,
                              std::unique_ptr<MsgReader>& msg_reader,
                              ProtoResponseCallbackEx& callback);

    void template_reduce_sum(const SendDataRequest& request,
                             std::unique_ptr<MsgReader>& msg_reader,
                             ProtoResponseCallbackEx& callback);

    void send_parameter(const SendParameterRequest& request,
                        std::unique_ptr<Msg_Reader> msg_reader,
                        ProtoResponseCallbackEx callback);

    void send_data(const SendDataRequest& request,
                   std::unique_ptr<MsgReader> msg_reader,
                   ProtoResponseCallbackEx callback);

    void set_config(const SetConfigRequest& request, ProtoReponseCallback callback);

    void get_status(const GetStatusRequest& request, ProtoResponseCallback callback);

    void set_status(const SetStatusRequest& request, ProtoResponseCallback callback);

    void do_operation(const DoOperationRequest& request, ProtoResponseCallback callback);

    void create_vector(const CreateVectorRequest& request, ProtoResponseCallback callback);

    void release_vector(const ReleaseVectorRequest& request, ProtoReponseCallback callback);


    void create_matrix(const CreateMatrixRequest& request, ProtoResponseCallback callback);

    void release_matrix(const ReleaseMatrixRequest& request, ProtoResponseCallback callback);

    void wait_pass_start(const WaitPassStartRequest& request, ProtoResponseCallback callback);

    void wait_pass_finish(const WaitFinishRequest& request, ProtoReponseCallback callback);

    void synchronize(const SynchronizeRequest& request, ProtoResponseCallback callback);

    void async_finish_pass(const SynchronizeRequest& request, ProtoResponseCallback callback);

    void load_value_vector(const LoadValueRequest& request, ProtoResponseCallback callback);

    void save_value_vector(const SaveValueRequest& request, ProtoResponseCallback callback);

public:
    bool init();

    void set_parameter(const SendParameterRequest& request,
                       std::vector<Buffer>& input_buffers,
                       SendParameterResponse* response,
                       std::vector<Buffer>* output_buffers);

    void async_sgd(const SendParameterRequest& request,
                   std::unique_ptr<Buffer>& input_buffers,
                   SendParameterResponse* response,
                   std::vector<Buffer>* output_buffers);

    void add_gradient(const SendParameterRequest& request,
                      std::vector<Buffer>& input_buffers,
                      SendParameterReponse* response,
                      std::vector<Buffer>* output_buffers);

    void get_parameter(const SendParameterRequest& request,
                       std::vector<Buffer>& input_buffers,
                       SendParameterResponse* response,
                       std::vector<Buffer>* output_buffers);

    void get_parameter_sparse(const SendParameterRequest& request,
                              std::vector<Buffer>& input_buffers,
                              SendParameterResponse* response,
                              std::vector<Buffer>* output_buffers);

protected:
    void merge_segments(BlockSegments* segments);

    void clear_unused_segments(CpuVector* vec);

    void read_all_blocks(MsgReader* msg_reader, std::vector<ParameterServer::Buffer>* buffers);

    const ParameterConfig& get_parameter_config(const ParameterBlock& block){
        CC_CHECK_LT(block.para_id(), -1UL) << "invalid parameter id:" << block.para_id();
        const auto it = _config_map.find(block.para_id());
        CC_CHECK(it != _config_map.end()) << "cannot find parameter id:" << block.para_id();
        return it->second;
    }

    const ParameterConfig& get_parameter_config(int64_t block_id) const{
        CC_CHECK(block_id >= 0 && block_id < (int64_t)_block_infos.size()) << "block idx out of range, id:" << block_id << " info size:" << _block_infos.size();
        return *(_block_infos[block_id].config);
    }

    template<class Response>
    bool is_valid_vector_handle(int64_t handle, Response* response){
        if(handle < 0 || (size_t)handle >= _vectors.size()){
            LOG(ERROR) << "Invalid vector handle " << handle;
            response->set_return_message(k_retmsg_invalid_vector_handle);
            return false;
        }
        return true;
    }

    template<class Response>
    bool is_valid_matrix_handle(int64_t handle, Response* response){
        if(handle < 0 || (size_t)handle >= _matrices.size()){
            LOG(ERROR) << "Invalid matrix handle " << handle;
            response->set_return_message(k_retmsg_invalid_matrix_handle);
            return false;
        }
        return true;
    }

    int64_t get_block_id(const ParameterBlock& block) const{
        BlockKey key(block.para_id(), block.block_id());
        auto it = _block_id_map.find(key);
        if(it == _block_id_map.end()){
            return -1;
        }
        return it->second;
    }

    void send_back_parameter(const ParameterBlock& block,
                             SendParameterResponse* response,
                             std::vector<Buffer>* output_buffers);

    void send_back_parameter(const ParameterBlock& block,
                             int parameter_type,
                             SendParameterReponse* response,
                             std::vector<Buffer>* output_buffers);

    typedef std::function<void(int64_t block_id, const VectorPtr vecs[])> ExecFunc;
    void parallel_exec_for_each_block(ExecFunc func);
    void block_traverse(BlockInfo& info,
                        const ParameterConfig& config,
                        int64_t offset,
                        size_t size,
                        const VectorPtr vecs[],
                        const ParameterOptimizer::TraverseCallback& callback);

public:
    typedef void (ParameterServer::*OperatorFunction)(const Operation& operation, OperationResult* result);

    static OperatorFunction op_funcs[];

    void op_sgd(const Operation& operation, OperationResult* result);

    void op_reset(const Operation& operation, OperationResult* result);

    void op_utv(const Operation& operation, OperationResult* result);

    void op_au_bv(const Operation& operation, OperationResult* result);

    void op_copy(const Operation& operation, OperationResult* result);

    void op_au(const Operation& operation, OperationResult* result);

    void op_au_bv_cw(const Operation& operation, OperationResult* result);

    void op_make_steepest_desc_dir(const Operation& operation, OperationResult* result);

    void op_fix_di_signs(const Operation& operation, OperationResult* result);

    void op_dir_deriv(const Operation& operation, OpertionResult* result);

    void op_fix_omega_signs(const Operation& operation, OperationResult* result);

    void op_cost(const Operation& operation, OperationResult* result);

    void op_start_pass(const Operation& operation, OperationResult* result);

    void op_finish_pass(const Operation& operation, OperationResult* result);

    void op_apply(const Operation& operation, OperationResult* result);

    void op_randomize(const Operation& operation, OperationResult* result);

    void op_load(const Operation& operation, OperationResult* result);

    void op_save(const Operation& operation, OperationResult* result);

    void tuning_sgd_mid_output();

    void tuning_sgd_finished();

    void tuning_async_sgd_mid_output();

    void tuning_async_sgd_finished();
};//class ParameterServer
}//namespace core
}//namespace ccml

#endif
