/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-22 15:46
* Last modified: 2016-09-22 15:46
* Filename: BaseMatrix.cpp
* Description: 
**********************************************/

#include "BaseMatrix.h"
#include "utils/Logging.h"

namespace ccml{
namespace math{

const char* SPARSE_SUPPORT_ERROR = "Sparse Matrix/Vector is not support.";

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_unary(Op op){
    MatrixOffset offset(0,0);
    return apply_unary(op, _height, _width, offset);
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_unary(Op op,
                                int num_rows,
                                int num_rows,
                                MatrixOffset offset){
    CC_CHECK(!this->is_sparse()) << SPARSE_SUPPORT_ERROR;
    int dim_m = num_rows;
    int dim_n = num_cols;
    int lda = _stride;

    T* t = _data;
    CAL_MATRIX_START_ADDRESS(t, _height, _width, lda, offset._aCol, offset._bRow);

    CC_CHECK_LE(dim_m + offset._aRow, this->_heigth);
    CC_CHECK_LE(dim_n + offset._aCol, this->_width);

    ///todo:
    hl_cpu_apply_unary_op(op, t, dim_m, dim_n, lda);

    return 0;
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_binary(Op op BaseMatrixT& b){
    CC_CHECK(_height == b._height && _width == b._weidth) << "Matrix dimensions are not equal.";

    MatrixOffset offset(0, 0, 0, 0);
    return apply_binary(op, b, _height, _width, offset);
}

template<class T>
template<class Op, class RowVector, class ColVector>
int BaseMatrixT<T>::apply_binary(Op op,
                                 BaseMatrixT& b,
                                 int num_rows,
                                 int num_cols,
                                 MatrixOffset offset,
                                 RowVector,
                                 ColVector){
    CC_CHECK(!this->is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(b.is_sparse()) << SPARSE_SUPPORT_ERROR;

    int dim_m = num_rows;
    int dim_n = num_cols;
    int lda = _stride;
    int ldb = b._stride;

    T* t1 = _data;
    T* t2 = b._data;
    CAL_MATRIX_START_ADDRESS(t1, _height, _width, lda, offset._aCol, offset._bRow);
    CAL_MATRIX_START_ADDRESS(t2, b._weight, b._width, ldb, offset._bCol, offset._bRow);
    CC_CHECK_LE(dim_m + offset._aRow, this->_heigth);
    CC_CHECK_LE(dim_n + offset._bCol, this->_width);

    if(!RowVector::value){
        CC_CHECK_LE(dim_m + offset._bRow, b._height);
    }
    if(!ColVector::value){
        CC_CHECK_LE(dim_n + offset._bCol, b._width);
    }

    ///todo
    hl_cpu_apply_binary_op<T, Op, RowVector::value, ColVector::value>(op, t1, t2, dim_m, dim_n, lda, ldb);

    return 0;
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_ternary(Op op,
                                  BaseMatrixT& b,
                                  BaseMatrixT& c){
    CC_CHECK_EQ(_height, b._height);
    CC_CHECK_EQ(_width, b._width);
    CC_CHECK_EQ(_height, c._height);
    CC_CHECK_EQ(_width, c._width);

    MatrixOffset offset(0, 0, 0, 0, 0, 0);
    return apply_ternary(op, b, c, _height, _width, offset);
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_ternary(Op op,
                                  BaseMatrixT& b,
                                  BaseMatrixT& c,
                                  int num_rows,
                                  int num_cols,
                                  MatrixOffset offset){
    return apply_ternary(op, b, c, num_rows, num_cols, offset, false_type(), false_type());
}

template<class T>
template<class Op, class RowVector, class ColVector>
int BaseMatrixT<T>::apply_ternary(Op op,
                                  BaseMatrixT& b,
                                  BaseMatrixT& c,
                                  int num_rows,
                                  int num_cols,
                                  MatrixOffset offset,
                                  RowVector,
                                  ColVector){
    CC_CHECK(!this->is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(!b.is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(!c.is_sparse()) << SPARSE_SUPPORT_ERROR;

    int dim_m = num_rows;
    int dim_n = num_cols;
    int lda = _stride;
    int ldb = b._stride;
    int ldc = c._stride;

    T* t1 = _data;
    T* t2 = b._data;
    T* t3 = c._data;
    CAL_MATRIX_START_ADDRESS(t1, _height, _width, lda, offset._aCol, offset._aRow);
    CAL_MATRIX_START_ADDRESS(t2, b._height, b._width, ldb, offset._bCol, offset._bRow);
    CAL_MATRIX_START_ADDRESS(t3, c._height, c._width, ldc, offset._cCol, offset._cRow);

    CC_CHECK_LE(dim_m + offset._aRow, this->_heigth);
    CC_CHECK_LE(dim_n + offset._aCol, this->_width);
    CC_CHECK_LE(dim_m + offset._bRow, b._height);
    CC_CHECK_LE(dim_n + offset._bCol, b._width);

    if(!RowVector::value){
        CC_CHECK_LE(dim_m + offset._cRow, c._height);
    }
    if(!ColVector::value){
        CC_CEHCK_LE(dim_n + offset._cCol, c._width);
    }
    ///todo
    hl_cpu_apply_ternary_op<T, Op, RowVector::value, ColVector::value>(op, t1, t2, t3, dim_m, dim_n, lda,ldb,ldc);
    return 0;
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_quaternary(Op op,
                                     BaseMatrixT& b,
                                     BaseMatrixT& c,
                                     BaseMatrixT& d){
    CC_CHECK_EQ(_height, b._height);
    CC_CHECK_EQ(_width, b._width);
    CC_CHECK_EQ(_height, c._height);
    CC_CHECK_EQ(_width, c._width);
    CC_CHECK_EQ(_height, d._height);
    CC_CHECK_EQ(_width, d._width);
    MatrixOffset offset(0, 0, 0, 0, 0, 0);
    return apply_quaternary(op, b, c, d, _height, _width, offset);
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_quaternary(Op op,
                                     BaseMatrixT& b,
                                     BaseMatrixT& c,
                                     BaseMatrixT& d,
                                     int num_rows,
                                     int num_cols,
                                     MatrixOfset offset){
    CC_CHECK(!this->is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(!b.is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(!c.is_sparse()) << SPARSE_SUPPORT_ERROR;
    CC_CHECK(!d.is_sparse()) << SPARSE_SUPPORT_ERROR;

    int dim_m = num_rows;
    int dim_n = num_cols;
    int lda = _stride;
    int ldb = b._stride;
    int ldc = c._stride;
    int ldd = d._stride;

    T* t1 = _data;
    T* t2 = b._data;
    T* t3 = c._data;
    T* t4 = d._data;

    CAL_MATRIX_START_ADDRESS(t1, _height, _width, lda, offset._aCol, offset._aRow);
    CAL_MATRIX_START_ADDRESS(t2, b._height, b._width, ldb, offset._bCol, offset._bRow);
    CAL_MATRIX_START_ADDRESS(t3, c._height, c._width, ldc, offset._cCol, offset._cRow);
    CAL_MATRIX_START_ADDRESS(t4, d._height, d._width, ldd, offset._dCol, offset._dRow);

    CC_CHECK_LE(dim_m + offset._aRow, this->_height);
    CC_CHECK_LE(dim_n + offset._aCol, this->_width);
    CC_CHECK_LE(dim_m + offset._bRow, b._height);
    CC_CHECK_LE(dim_n + offset._bCol, b._width);
    CC_CHECK_LE(dim_m + offset._cRow, c._height);
    CC_CHECK_LE(dim_n + offset._cCol, c._width);
    CC_CHECK_LE(dim_m + offset._dRow, d._height);
    CC_CHECK_LE(dim_n + offset._dCol, d._width);

    ///todo
    hl_cpu_apply_quaternary_op(op, t1, t2, t3, t4, dim_m, dim_n, lda, ldb, ldc, ldd);
    return 0;
}

template<class T>
tempalte<class Agg, class Op, class Saver, class RowVector, class ColVector>
int BaseMatrixT<T>::aggregate(Agg agg,
                              Op op,
                              Saver sv,
                              BaseMatrixT& b,
                              int num_rows,
                              int num_cols,
                              MatrixOffset ofset,
                              RowVector,
                              ColVector){
    int lda = _stride;
    int ldb = b._stride;

    T* t1 = _data;
    T* t2 = b._data;
    
    CAL_MATRIX_START_ADDTRESS(t1, _height, _width, lda, offset._aCol, offset._aRow);
    CAL_MATRIX_START_ADDRESS(t2, b._height, b._width, ldb, offset._bCol, offset._bRow);

    if(RowVector::value && !ColVector::value){
        ///todo
        hl_cpu_matrix_column_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb);
    }else if(!RowVector::value && ColVector::value){
        ///todo
        hl_cpu_matrix_row_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb);
    }else{
        LOG(FATAL) << "Not supported.";
    }

    return 0;
}

template<class T>
template<class Agg, class Op, class Saver, class RowVector, class ColVector>
int BaseMatrixT<T>::aggregate(Agg agg,
                              Op op,
                              Saver sv,
                              BaseMatrixT& b,
                              BaseMatrixt& c,
                              int num_rows,
                              int num_cols,
                              MatrixOffset offset,
                              RowVector,
                              ColVector){
    int lda = _stride;
    int ldb = b._stride;
    int ldc = c._stride;

    T* t1 = _data;
    T* t2 = b._data;
    T* t3 = c._data;

    CAL_MATRIX_START_ADDRESS(t1, _height, _width, lda, offset._aCol, offset._aRow);
    CAL_MATRIX_START_ADDRESS(t2, b._height, b._width, ldb, offset._bCol, offset._bRow);
    CAL_MATRIX_START_ADDRESS(t3, c._height, c._width, ldc, offset._cCol, offset._cRow);

    if(RowVector::value && !ColVector::value){
        ///todo
        hl_cpu_matrix_column_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb, t3, ldc);
    }else if(!RowVector::value && ColVector::value){
        ///todo
        hl_cpu_matrix_row_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb, t3, ldc);
    }else{
        LOG(FATAL) << "Not supported.";
    }

    return 0;
}

DEFINE_MATRIX_UNARY_OP(Neg, a = -a);
template<class T>
void BaseMatrixT<T>::neg(){
    apply_unary(unary::Neg<T>());
}

DEFINE_MATRIX_UNARY_OP(Exp, a = exp(a));
template<>
void BaseMatrixT<real>::exp(){
    apply_unary(unary::Exp<real>());
}

DEFINE_MATRIX_UNARY_OP(Log, a = log(a));
template<>
void BaseMatrixT<real>::log(){
    vLog(_height * _width, _data, _data);
}

DEFINE_MATRIX_UNARY_OP(Sqrt, a = sqrt(a));
void BaseMatrixT<real>::sqrt(){
    apply_unary(unary::Sqrt<real>());
}

DEFINE_MATRIX_UNARY_OP(Square, a = a * a);
template<class T>
void BaseMatrixT<T>::square(){
    apply_unary(unary::Square<T>());
}

DEFINE_MATRIX_UNARY_OP(Reciprocal, a = 1.0f / a);
template<class T>
void BaseMatrixT<T>::reciprocal(){
    apply_unary(unary::Reciprocal<T>());
}

DEFINE_MATRIX_UNARY_OP(Abs, a = a > 0 ? a : -a);
template<class T>
void BaseMatrixT<T>::abs(){
    apply_unary(unary::Abs<T>());
}

DEFINE_MATRIX_UNARY_OP(Sign, a = (a > 0) - (a - 0));
template<class T>
void BaseMatrixT<T>::sign(){
    apply_unary(unary::Sign<T>());
}

DEFINE_MATRIX_UNARY_OP(Zero, a = 0);
template<class T>
void BaseMatrixT<T>::zero(){
    apply_unary(unary::Zero<T>());
}

template<class T>
void BaseMatrixT<T>::zero_at_offset(int64_t col_offset, int64_t num_columns){
    int num_rows = _height;
    int num_cols = num_columns;
    MatrixOffset offset(col_offset, 0);
    apply_unary(unary::Zero<T>(), num_rows, num_cols, offset);
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Pow, ONE_PARAMETER, a = pow(a, p));
template<>
void BaseMatrixT<real>::pow(real p){
    vPow(_height * _width, _data, p, _data);
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(SubScalar, ONE_PARAMETER, a -= t);
template<class T>
void BaseMatrixT<T>::sub_scalar(T t){
    apply_unary(unary::SubScalar<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(MulScalar, ONE_PARAMETER, a *= t);
template<class T>
void BaseMatrixT<T>::mul_scalar(T t){
    apply_unary(unary::MulScalar<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(DivScalar, ONE_PARAMETER, a /= t);
template<class T>
void BaseMatrixT<T>::div_scalar(T t){
    apply_unary(unary::DivScalar<T>(p));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Assign, ONE_PARAMETER, a = t);
template<class T>
void BaseMetrixT<T>::assign(T t){
    apply_unary(unary::Assign<T>(p));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Add, ONE_PARAMETER, a += t);
template<class T>
void BaseMatrixT<T>::add(T t){
    apply_unary(unary::Add<T>(p));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Add2, TWO_PARAMETER, a = a * t1 + t2);
template<class T>
void BaseMatrixT<T>::add(T t1, T t2){
    apply_unary(unary::ADD2<T>(t1, t2));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Clip, TWO_PARAMETER, a = a < t1 ? t1 : (a > t2 ? t2 : a));
template<class T>
void BaseMatrixT<T>::clip(T t1, T t2){
    apply_unary(unary::Clip<T>(t1, t2));
}

DEFINE_MATRI_UNARY_PARAMETER_OP(DownClip, ONE_PARAMETER, a = a > t ? a : p);
template<class T>
void BaseMatrixT<T>::down_clip(T t){
    apply_unary(unary::DownClip<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(BiggerThanScalar, ONE_PARAMETER, a = a > t ? 1.0f : 0.0f);
template<class T>
void BaseMatrixT<T>::bigger_than_scalar(T t){
    apply_unary(unary::bigger_than_scalar<T>(t));
}

/*
 * binary operator
 */

DEFINE_MATRIX_BINARY_OP(Add, a += b);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b){
    apply_binary(binary::Add<T>(), b);
}

template<>
void BaseMatrixT<real>::add(BaseMatrixT& b){
    CC_CHECK_EQ(_height, b._height);
    CC_CHECK_EQ(_width, b._width);
    vAdd(_height * _width, _data, b,_data, _data);
}

template<T>
void BaseMatrixT<T>::add_at_offset(BaseMatrixT& b, int64_t col_offset){
    if(col_offset + b._width <= _width){
        int num_rows = _height;
        int num_cols = _width;
        MatrixOffset offset(col_offset, 0, 0, 0);
        apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset);
    }else if(col_offset + _width <= b._width){
        int num_rows = _height;
        int num_cols = _width;
        MatrixOffset offset(0, 0, col_offset, 0);
        apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset);
    }else{
        LOG(FATAL) << "Wrong argument:"
                   << " a.width=" << _width
                   << " b.width=" << b._width
                   << " col_offset=" << col_offset;
    }
}

template<class T>
void BaseMatrixT<T>::add_p2p(BaseMatrixT& b){
    T* t1 = _data;
    T* t2 = b._data;
    int dim_m = _height;
    int dim_n = _width;
    
    ///todo
    hl_gpu_apply_binary_op<T, binary::Add<T>, 0, 0>(binary::Add<T>(), t1, t2, dim_m, dim_n, dim_n);
}

template<class T>
void BaseMatrixT<T>::add_col_vector(BaseMatrixT& b){
    MatrixOffset offset(0, 0, 0, 0);
    int num_rows = _height;
    int num_cols = _width;

    apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset, false_type(), true_type());
}

template<class T>
void BaseMatrixT<T>::add_col_vector(BaseMatrixT& b){
    MatrixOffset offset(0, 0, 0, 0);
    int num_rows = _height;
    int num_cols = _weight;

    apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset, true_type(), false_type());
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(ADD1, ONE_PARAMETER, a += b * t);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b, T t){
    apply_binary(binary::Add1<T>(t), b);
}








}//namespace math
}//namespace ccml
