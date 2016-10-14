/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-13 17:21
* Last modified: 2016-10-13 17:21
* Filename: Vector.h
* Description: 
**********************************************/

#pragma once

#include "BaseMatrix.h"
#include <memory>


namespace ccml{
namespace math{

template<class T>
class BaseVector : public BaseMatrixT<T>{
public:
    BaseVector(size_t size, T* data) : BaseMatrixT<T>(1, size, data, false), _size(this->_width){}

    ~BaseVector(){}

protected:
    size_t& _size;
};//class BaseVector


template<class T>
class VectorT : public BaseVector<T>{
protected:
    VectorT(size_t size, T* data) : BaseVector<T>(size, data){}

    VectorT(size_t size,
            MemoryHandlePtr memory_handle,
            size_t offset) : BaseVectorT(size, reinterpret_cast<T*>(memory_handle->get_buf()) + offset){
        _memory_handle = memory_handle;
    }

public:
    virtual ~VectorT(){}

    static std::shared_ptr<VectorT<T>> create(size_t size);

    static std::shared_ptr<VectorT<T>> create(T* data, size_t size);

    static std::shared_ptr<VectorT<T>> create(size_t size,
                                              MemoryHandlePtr memory_handle,
                                              size_t offset = 0);

    static std::shared_ptr<VectorT<T>> create_parallel_vector(size_t size, SyncThreadPool* pool = nullptr);

    size_t get_size() const {
        return this->_size;
    }

    const T* get_data() const {
        return this->_data;
    }

    T* get_data(){
        return this->_data;
    }

    virtual void zero_mem() = 0;

    virtual void reset(const T& value) = 0;

    virtual void fill_sequence() = 0;

    MemoryHandlePtr get_memory_handle() const {
        return _memory_handle;
    }

    void resize(size_t new_size){
        if(!_memory_handle || new_size * sizeof(T) > _memory_handle->get_alloc_size()){
            _memory_handle = new_memory(new_size * sizeof(T));
            this->_data = reinterpret_cast<T*>(_memory_handle->get_buf());
        }
        this->_size = new_size;
    }

    static void resize_or_create(std::shared_ptr<VectorT<T>>& vec, size_t size){
        if(vec){
            vec->resize(size);
        }else{
            vec = create(size);
        }
    }

    virtual MemoryHandlePtr new_memory(size_t size) = 0;

    void sub_vec_from(const VectorT<T>& src,
                      size_t start,
                      size_t size){
        CC_CHECK_LT(start, src._size);
        CC_CHECK_LE(start + size, src._size);

        BaseVector<T>::_size = size;
        BaseVector<T>::_data = const_cast<T*>(src._data) + start;
    }

    std::shared_ptr<VectorT<T>> sub_vec(size_t start, size_t size){
        CHECK_LE(start + size, static_cast<size_t>(get_size()));
        return VectorT<T>::create(get_data() + start, size);
    }

    void sub_vec_from(const T* src,
                      size_t start,
                      size_t size){
        BaseVector<T>::_size = size;
        BaseVector<T>::_data = const_cast<T*>(src) + start;
    }

    void sub_vec_from(const VectorT<T>& src, std::pair<size_t, size_t> interval){
        sub_vec_from(src, interval.first, interval.second - interval.first);
    }

    virtual void copy_from(const VectorT<T>& src) = 0;




};//class VectorT

typedef VectorT<real> Vector;
typedef CpuVectorT<real> CpuVector;

typedef std::shared_ptr<Vector> VectorPtr;
typedef std::shared_ptr<CpuVector> CpuVectorPtr;


}//namespace math
}//namespace ccml
