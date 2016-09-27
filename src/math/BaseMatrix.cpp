/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-22 15:46
* Last modified: 2016-09-22 15:46
* Filename: BaseMatrix.cpp
* Description: 
**********************************************/

#include "BaseMatrix.h"
#include "MatrixOps.h"
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
                                int num_cols,
                                MatrixOffset& offset){
    CC_CHECK(!this->is_sparse()) << SPARSE_SUPPORT_ERROR;
    int dim_m = num_rows;
    int dim_n = num_cols;
    int lda = _stride;

    T* t = _data;
    CAL_MATRIX_START_ADDRESS(t, _height, _width, lda, offset._aCol, offset._bRow);

    CC_CHECK_LE(dim_m + offset._aRow, this->_height);
    CC_CHECK_LE(dim_n + offset._aCol, this->_width);

    ///todo:
    //hl_cpu_apply_unary_op(op, t, dim_m, dim_n, lda);

    return 0;
}

template<class T>
template<class Op>
int BaseMatrixT<T>::apply_binary(Op op, BaseMatrixT& b){
    CC_CHECK(_height == b._height && _width == b._width) << "Matrix dimensions are not equal.";

    MatrixOffset offset(0, 0, 0, 0);
    return apply_binary(op, b, _height, _width, offset);
}

template<class T>
template<class Op, class RowVector, class ColVector>
int BaseMatrixT<T>::apply_binary(Op op,
                                 BaseMatrixT& b,
                                 int num_rows,
                                 int num_cols,
                                 MatrixOffset& offset,
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
    CC_CHECK_LE(dim_m + offset._aRow, this->_height);
    CC_CHECK_LE(dim_n + offset._bCol, this->_width);

    if(!RowVector::value){
        CC_CHECK_LE(dim_m + offset._bRow, b._height);
    }
    if(!ColVector::value){
        CC_CHECK_LE(dim_n + offset._bCol, b._width);
    }

    ///todo
    //hl_cpu_apply_binary_op<T, Op, RowVector::value, ColVector::value>(op, t1, t2, dim_m, dim_n, lda, ldb);

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
                                  MatrixOffset& offset){
    return apply_ternary(op, b, c, num_rows, num_cols, offset, false_type(), false_type());
}

template<class T>
template<class Op, class RowVector, class ColVector>
int BaseMatrixT<T>::apply_ternary(Op op,
                                  BaseMatrixT& b,
                                  BaseMatrixT& c,
                                  int num_rows,
                                  int num_cols,
                                  MatrixOffset& offset,
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

    CC_CHECK_LE(dim_m + offset._aRow, this->_height);
    CC_CHECK_LE(dim_n + offset._aCol, this->_width);
    CC_CHECK_LE(dim_m + offset._bRow, b._height);
    CC_CHECK_LE(dim_n + offset._bCol, b._width);

    if(!RowVector::value){
        CC_CHECK_LE(dim_m + offset._cRow, c._height);
    }
    if(!ColVector::value){
        CC_CHECK_LE(dim_n + offset._cCol, c._width);
    }
    ///todo
    //hl_cpu_apply_ternary_op<T, Op, RowVector::value, ColVector::value>(op, t1, t2, t3, dim_m, dim_n, lda,ldb,ldc);
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
                                     MatrixOffset& offset){
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
    //hl_cpu_apply_quaternary_op(op, t1, t2, t3, t4, dim_m, dim_n, lda, ldb, ldc, ldd);
    return 0;
}

template<class T>
template<class Agg, class Op, class Saver, class RowVector, class ColVector>
int BaseMatrixT<T>::aggregate(Agg agg,
                              Op op,
                              Saver sv,
                              BaseMatrixT& b,
                              int num_rows,
                              int num_cols,
                              MatrixOffset& offset,
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
        //hl_cpu_matrix_column_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb);
    }else if(!RowVector::value && ColVector::value){
        ///todo
        //hl_cpu_matrix_row_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb);
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
                              BaseMatrixT& c,
                              int num_rows,
                              int num_cols,
                              MatrixOffset& offset,
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
        //hl_cpu_matrix_column_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb, t3, ldc);
    }else if(!RowVector::value && ColVector::value){
        ///todo
        //hl_cpu_matrix_row_op(agg, op, sv, num_rows, num_cols, t1, t2, ldb, t3, ldc);
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
    //vLog(_height * _width, _data, _data);
}

DEFINE_MATRIX_UNARY_OP(Sqrt, a = sqrt(a));
template<>
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

DEFINE_MATRIX_UNARY_PARAMETER_OP(Pow, ONE_PARAMETER, a = pow(a, t));
template<>
void BaseMatrixT<real>::pow(real t){
//    vPow(_height * _width, _data, t, _data);
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
    apply_unary(unary::DivScalar<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Assign, ONE_PARAMETER, a = t);
template<class T>
void BaseMatrixT<T>::assign(T t){
    apply_unary(unary::Assign<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Add, ONE_PARAMETER, a += t);
template<class T>
void BaseMatrixT<T>::add(T t){
    apply_unary(unary::Add<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Add2, TWO_PARAMETER, a = a * t1 + t2);
template<class T>
void BaseMatrixT<T>::add(T t1, T t2){
    apply_unary(unary::Add2<T>(t1, t2));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(Clip, TWO_PARAMETER, a = a < t1 ? t1 : (a > t2 ? t2 : a));
template<class T>
void BaseMatrixT<T>::clip(T t1, T t2){
    apply_unary(unary::Clip<T>(t1, t2));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(DownClip, ONE_PARAMETER, a = a > t ? a : t);
template<class T>
void BaseMatrixT<T>::down_clip(T t){
    apply_unary(unary::DownClip<T>(t));
}

DEFINE_MATRIX_UNARY_PARAMETER_OP(BiggerThanScalar, ONE_PARAMETER, a = a > t ? 1.0f : 0.0f);
template<class T>
void BaseMatrixT<T>::bigger_than_scalar(T t){
    apply_unary(unary::BiggerThanScalar<T>(t));
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
    //vAdd(_height * _width, _data, b, _data, _data);
}

template<class T>
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
    //hl_gpu_apply_binary_op<T, binary::Add<T>, 0, 0>(binary::Add<T>(), t1, t2, dim_m, dim_n, dim_n);
}

template<class T>
void BaseMatrixT<T>::add_col_vector(BaseMatrixT& b){
    MatrixOffset offset(0, 0, 0, 0);
    int num_rows = _height;
    int num_cols = _width;

    apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset, false_type(), true_type());
}

template<class T>
void BaseMatrixT<T>::add_row_vector(BaseMatrixT& b){
    MatrixOffset offset(0, 0, 0, 0);
    int num_rows = _height;
    int num_cols = _width;

    apply_binary(binary::Add<T>(), b, num_rows, num_cols, offset, true_type(), false_type());
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(Add1, ONE_PARAMETER, a += b * t);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b, T t){
    apply_binary(binary::Add1<T>(t), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(Pow, ONE_PARAMETER, a = pow(b, t));
template<>
void BaseMatrixT<real>::pow(BaseMatrixT& b, real t){
//    vPow(_height * _width, b._data, t, _data);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(Add2, TWO_PARAMETER, a = t1 * a + t2 * b);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b, T t1, T t2){
    apply_binary(binary::Add2<T>(t1, t2), b);
}

template<class T>
void BaseMatrixT<T>::add_bias(BaseMatrixT& b, T scale){
    MatrixOffset offset(0, 0, 0, 0);
    int num_rows = _height;
    int num_cols = _width;

    apply_binary(binary::Add1<T>(scale), b, num_rows, num_cols, offset, true_type(), false_type());
}

DEFINE_MATRIX_BINARY_OP(Sub , a -= b);
template<class T>
void BaseMatrixT<T>::sub(BaseMatrixT& b){
    apply_binary(binary::Sub<T>(), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(Sub1, ONE_PARAMETER, a -= b * t);
template<class T>
void BaseMatrixT<T>::sub(BaseMatrixT& b, T t){
    apply_binary(binary::Sub1<T>(t), b);
}

DEFINE_MATRIX_BINARY_OP(Relu, b = a > 0.0f ? a : 0.0f);
template<class T>
void BaseMatrixT<T>::relu(BaseMatrixT& b){
    apply_binary(binary::Relu<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(ReluDerivative, a *= (b > 0.0f ? 1.0f : 0.0f));
template<class T>
void BaseMatrixT<T>::relu_derivative(BaseMatrixT& b){
    apply_binary(binary::ReluDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(SoftRelu, const T THRESHOLD = 40.0;\
    b = log(1.0 + exp(( a > THRESHOLD) ? THRESHOLD : ( (a < -THRESHOLD) ? -THRESHOLD : a))));
template<>
void BaseMatrixT<real>::soft_relu(BaseMatrixT& b){
    apply_binary(binary::SoftRelu<real>(), b);
}

DEFINE_MATRIX_BINARY_OP(SoftReluDerivative, const T THRESHOLD = 40.0;\
    a *= (1.0 - exp(-1.0 * ((b > THRESHOLD) ? THRESHOLD: ((b < -THRESHOLD) ? -THRESHOLD : b)))));
template<>
void BaseMatrixT<real>::soft_relu_derivative(BaseMatrixT& b){
    apply_binary(binary::SoftReluDerivative<real>(), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(Brelu, TWO_PARAMETER, b = a > t1 ? a : t1;\
        b = b < t2 ? b : t2);
template<class T>
void BaseMatrixT<T>::brelu(BaseMatrixT& b){
    int t1 = 0, t2 = 24;
    apply_binary(binary::Brelu<T>(t1, t2), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(BreluDerivative, TWO_PARAMETER, a *= (b > t1 && b < t2) ? 1.0 : 0.0);
template<class T>
void BaseMatrixT<T>::brelu_derivative(BaseMatrixT& b){
    int t1 = 0, t2 = 24;
    apply_binary(binary::BreluDerivative<T>(t1, t2), b);
}

DEFINE_MATRIX_BINARY_OP(Square, b = a * a);
template<class T>
void BaseMatrixT<T>::square(BaseMatrixT& b){
    apply_binary(binary::Square<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(SquareDerivative, a *= 2.0 * b);
template<class T>
void BaseMatrixT<T>::square_derivative(BaseMatrixT& b){
    apply_binary(binary::SquareDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(Tanh, b = 2.0 / (1.0 + exp(-2 * b)) - 1.0);
template<>
void BaseMatrixT<real>::tanh(BaseMatrixT& b){
    apply_binary(binary::Tanh<real>(), b);
}

DEFINE_MATRIX_BINARY_OP(TanhDerivative, a *= 1- b * b);
template<class T>
void BaseMatrixT<T>::tanh_derivative(BaseMatrixT& b){
    apply_binary(binary::TanhDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(ScaledTanh, TWO_PARAMETER, b = t1 * (2.0 / (1.0 + exp(-2 * t2 * a)) - 1.0));
template<>
void BaseMatrixT<real>::scaled_tanh(BaseMatrixT& b, real t1, real t2){
    apply_binary(binary::ScaledTanh<real>(t1, t2), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(ScaledTanhDerivative, TWO_PARAMETER, a *= t2 * (t1 - b * b));
template<class T>
void BaseMatrixT<T>::scaled_tanh_derivative(BaseMatrixT& b, T t1, T t2){
    apply_binary(binary::ScaledTanhDerivative<T>(t1 * t1, t2 / t1), b);
}

DEFINE_MATRIX_BINARY_OP(ReciprocalDerivative, a *= -b * b);
template<class T>
void BaseMatrixT<T>::reciprocal_derivative(BaseMatrixT& b){
    apply_binary(binary::ReciprocalDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(Abs, b = a > 0.0f ? a : -a);
template<class T>
void BaseMatrixT<T>::abs(BaseMatrixT& b){
    apply_binary(binary::Abs<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(AbsDerivative, a = (b>0) ? a : (b<0)? -a : 0);
template<class T>
void BaseMatrixT<T>::abs_derivative(BaseMatrixT& b){
    apply_binary(binary::AbsDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(Sigmoid, const T THRESHOLD_MIN = -40.0; const T THRESHOLD_MAX = 13.0; T tmp = (a<THRESHOLD_MIN) ? THRESHOLD_MIN :((a>THRESHOLD_MAX) ? THRESHOLD_MAX : a); b = 1.0f /(1.0f + exp(-tmp)));
template<>
void BaseMatrixT<real>::sigmoid(BaseMatrixT& b){
    size_t num_samples = this->_height;
    size_t dim = this->_width;
    CC_CHECK_EQ(b._height, num_samples);
    CC_CHECK_EQ(b._width, dim);
    const real* in = this->_data;
    real* out = b._data;

    const float THRESHOLD_MIN = -40.0;
    const float THRESHOLD_MAX = 13.0;
    for(size_t i = 0; i < num_samples * dim; i++){
        real temp = in[i];
        temp = (temp < THRESHOLD_MIN) ? THRESHOLD_MIN : ((temp > THRESHOLD_MAX) ? THRESHOLD_MAX : temp);
        out[i] = -temp;
    }

    //vExp(num_samples * dim, out, out);//out = exp(out)

    for(size_t i = 0; i < num_samples * dim; i++){
        out[i] = 1 / (1 + out[1]);
    }
}

DEFINE_MATRIX_BINARY_OP(SigmoidDerivative, a *= b * (1 - b));
template<class T>
void BaseMatrixT<T>::sigmoid_derivative(BaseMatrixT& b){
    apply_binary(binary::SigmoidDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(ExpDerivative, a *= b);
template<class T>
void BaseMatrixT<T>::exp_derivative(BaseMatrixT& b){
    apply_binary(binary::ExpDerivative<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(Sign, b = a > 0.0f ? 1.0f : -1.0f);
template<class T>
void BaseMatrixT<T>::sign(BaseMatrixT& b){
    apply_binary(binary::Sign<T>(), b);
}

DEFINE_MATRIX_BINARY_OP(Exp, a = exp(b));
template<>
void BaseMatrixT<real>::exp(BaseMatrixT& b){
    apply_binary(binary::Exp<real>(), b);
}

DEFINE_MATRIX_BINARY_OP(Log, a = log(b));
template<>
void BaseMatrixT<real>::log(BaseMatrixT& b){
    //vLog(_height * _width, b_data, _data);
}

DEFINE_MATRIX_BINARY_OP(Sqrt, a = sqrt(b));
template<>
void BaseMatrixT<real>::sqrt(BaseMatrixT& b){
    apply_binary(binary::Sqrt<real>(), b);
}

DEFINE_MATRIX_BINARY_OP(InvSqrt, a = 1.0f / sqrt(b));
template<>
void BaseMatrixT<real>::inv_sqrt(BaseMatrixT& b){
    CC_CHECK_EQ(_height, b._height);
    CC_CHECK_EQ(_width, b._width);
    //vInvSqrt(_height * _width, b._data, _data);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(IsEqualTo, ONE_PARAMETER, a = (b == t));
template<class T>
void BaseMatrixT<T>::is_equal_to(BaseMatrixT& b, T t){
    apply_binary(binary::IsEqualTo<T>(t), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(AddScalar, ONE_PARAMETER, a = b + t);
template<class T>
void BaseMatrixT<T>::add_scalar(BaseMatrixT& b, T t){
    apply_binary(binary::AddScalar<T>(t), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(SubScalar, ONE_PARAMETER, a = b - t);
template<class T>
void BaseMatrixT<T>::sub_scalar(BaseMatrixT& b, T t){
    apply_binary(binary::SubScalar<T>(t), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(MulScalar, ONE_PARAMETER, a = b * t);
template<class T>
void BaseMatrixT<T>::mul_scalar(BaseMatrixT& b, T t){
    apply_binary(binary::MulScalar<T>(t), b);
}

DEFINE_MATRIX_BINARY_PARAMETER_OP(DivScalar, ONE_PARAMETER, a = b / t);
template<class T>
void BaseMatrixT<T>::div_scalar(BaseMatrixT& b, T t){
    apply_binary(binary::DivScalar<T>(t), b);
}

/*
 * ternary operator
 */

DEFINE_MATRIX_TERNARY_OP(SoftCrossEntropy, a = -c * log(b) - (1 - c) * log(1 - b));
template<>
void BaseMatrixT<real>::soft_cross_entropy(BaseMatrixT& b, BaseMatrixT& c){
    apply_ternary(ternary::SoftCrossEntropy<real>(), b, c);
}

DEFINE_MATRIX_TERNARY_OP(SoftCrossEntropyBp, a += (b - c) / (b * (1 - b)));
template<class T>
void BaseMatrixT<T>::soft_cross_entropy_bp(BaseMatrixT& b, BaseMatrixT& c){
    apply_ternary(ternary::SoftCrossEntropyBp<T>(), b, c);
}

DEFINE_MATRIX_TERNARY_OP(BinaryLabelCrossEntropy, a = c > 0.5 ? -log(b) : -log(1.0 - b));
template<>
void BaseMatrixT<real>::binary_label_cross_entropy(BaseMatrixT& b, BaseMatrixT& c){
    CC_CHECK_EQ(_height, b._height);
    CC_CHECK_EQ(_height, c._height);
    CC_CHECK_EQ(_width, b._width);
    CC_CHECK_EQ(_width, c._width);

    size_t size = _height * _width;
    real* out = b._data;
    real* label = b._data;
    real* cost = _data;

    for(size_t i = -0; i < size; i++){
        cost[i] = label[i] > 0.5 ? out[i] : 1.0 - out[i];
    }
//    vLog(size, cost, cost);
    for(size_t i = 0; i < size; i++){
        cost[i] *= -1.0;
    }
}

DEFINE_MATRIX_TERNARY_OP(BinaryLabelCrossEntropyBp, a += c > 0.5 ? -1.0/b : 1.0/(1.0-b))
template<class T>
void BaseMatrixT<T>::binary_label_cross_entropy_bp(BaseMatrixT& b, BaseMatrixT& c){
    apply_ternary(ternary::BinaryLabelCrossEntropyBp<T>(), b, c);
}

DEFINE_MATRIX_TERNARY_OP(Add, a = b + c);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b, BaseMatrixT& c){
    apply_ternary(ternary::Add<T>(), b, c);
}

DEFINE_MATRIX_TERNARY_PARAMETER_OP(Add1, TWO_PARAMETER, a = t1 * b + t2 * c);
template<class T>
void BaseMatrixT<T>::add(BaseMatrixT& b,
                         T t1,
                         BaseMatrixT& c,
                         T t2){
    apply_ternary(ternary::Add1<T>(t1, t2), b, c);
}
}//namespace math
}//namespace ccml
