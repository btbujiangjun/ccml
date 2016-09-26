/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-20 12:04
* Last modified: 2016-09-20 12:04
* Filename: BaseMatrix.h
*********************************************/
#ifndef CCML_MATH_BASEMATRIX_H
#define CCML_MATH_BASEMATRIX_H

#include "utils/TypeDefs.h"
#include <cstddef>
#include <stdint.h>
namespace ccml{
namespace math{

template<class T, T v>
struct bool_constant{
    static const T value = v;
};
typedef bool_constant<bool, false> false_type;
typedef bool_constant<bool, true> true_type;

#define CAL_MATRIX_START_ADDRESS(address, height, width, ld, col, row) \
    CC_CHECK_LE(col, width); \
    CC_CHECK_LE(row, height); \
    address += row * ld + col;

class MatrixOffset{
public:
    size_t _aCol;
    size_t _aRow;

    size_t _bCol;
    size_t _bRow;
    
    size_t _cCol;
    size_t _cRow;
    
    size_t _dCol;
    size_t _dRow;

    MatrixOffset(size_t aCol = 0,
                 size_t aRow = 0,
                 size_t bCol = 0,
                 size_t bRow = 0,
                 size_t cCol = 0,
                 size_t cRow = 0,
                 size_t dCol = 0,
                 size_t dRow = 0
            ) : _aCol(aCol),
                _aRow(bRow),
                _bCol(bCol),
                _bRow(bRow),
                _cCol(cCol),
                _cRow(cRow),
                _dCol(dCol),
                _dRow(dRow) {}
};
    
template<class T>
class BaseMatrixT{
public:
    size_t _height;
    size_t _width;
    size_t _stride;
    T* _data;
    bool _trans;

public:
    BaseMatrixT(size_t height,
               size_t width,
               T* data,
               bool trans):
        _height(height),
        _width(width),
        _stride(width),
        _data(data),
        _trans(trans) {}
    BaseMatrixT(size_t height,
               size_t width,
               size_t stride,
               T* data,
               bool trans):
        _height(height),
        _width(width),
        _stride(stride),
        _data(data),
        _trans(trans) {}
    BaseMatrixT(BaseMatrixT& mat) : 
        _height(mat._height),
        _width(mat._width),
        _stride(mat._stride),
        _data(mat._data),
        _trans(mat._trans) {}


    virtual ~BaseMatrixT() {}

    /// make sure that the size of data is at least height * width
    void set_data(T* data){
        _data = data;
    }

    template<class Op>
    int apply_unary(Op op);

    template<class Op>
    int apply_unary(Op op,
                    int num_rows,
                    int num_cols,
                    MatrixOffset& offset);
    
    template<class Op>
    int apply_binary(Op op, BaseMatrixT& b);

    template <class Op, class RowVector, class ColVector>
    int apply_binary(Op op,
                     BaseMatrixT& b,
                     int num_rows,
                     int num_cols,
                     MatrixOffset& offset,
                     RowVector,
                     ColVector);

    template<class Op>
    int apply_binary(Op op,
                     BaseMatrixT& b,
                     int num_rows,
                     int num_cols,
                     MatrixOffset& offset);

    template<class Op>
    int apply_ternary(Op op,
                      BaseMatrixT& b,
                      BaseMatrixT& c);

    template<class Op, class RowVector, class ColVector>
    int apply_ternary(Op op,
                      BaseMatrixT& b,
                      BaseMatrixT& c,
                      int num_rows,
                      int num_cols,
                      MatrixOffset& offset,
                      RowVector,
                      ColVector);
    
    template<class Op>
    int apply_ternary(Op op,
                      BaseMatrixT& b,
                      BaseMatrixT& c,
                      int num_rows,
                      int num_cols,
                      MatrixOffset& offset);

    template<class Op>
    int apply_quaternary(Op op,
                         BaseMatrixT& b,
                         BaseMatrixT& c,
                         BaseMatrixT& d);
    
    template<class Op>
    int apply_quaternary(Op op,
                         BaseMatrixT& b,
                         BaseMatrixT& c,
                         BaseMatrixT& d,
                         int num_rows,
                         int num_cols,
                         MatrixOffset& offset);


    template<class Agg,class Op, class Saver, class RowVector, class ColVector>
    int aggregate(Agg agg,
                  Op op,
                  Saver sv,
                  BaseMatrixT& b,
                  BaseMatrixT& c,
                  int num_rows,
                  int num_cols,
                  MatrixOffset& offset,
                  RowVector,
                  ColVector);

    template<class Agg>
    int apply_row(Agg agg, BaseMatrixT& b);

    template<class Agg, class Saver>
    int apply_row(Agg agg,
                  Saver sv,
                  BaseMatrixT& b);

    template<class Agg>
    int apply_col(Agg agg, BaseMatrixT& b);
    
    template<class Agg, class Saver>
    int apply_col(Agg agg, Saver sv, BaseMatrixT& b);
   

    const T* row_buf(size_t row) const{
        return _data + _width * row;
    }

    T* row_buf(size_t row){
        return _data + _width * row;
    }


    /*
     * unary operator
     */
    void neg();
    void exp();
    void exp(BaseMatrixT& b);
    void exp_derivative(BaseMatrixT& b);
    void pow(T t);
    void pow(BaseMatrixT& b);
    void log();
    void log(BaseMatrixT& b);
    void sqrt();
    void sqrt(BaseMatrixT& b);
    void square();
    void reciprocal();
    void abs();
    void sign();
    void sign(BaseMatrixT& b);
    void zero();

    void zero_at_offset(int64_t col_offset, int64_t num_cols);
    void one();
    void sub_scalar(T t);
    void mul_scalar(T t);
    void div_scalar(T t);
    void assign(T t);
    void add(T t);
    void add(BaseMatrixT& b);
    void add(BaseMatrixT& b, T t);
    void add(BaseMatrixT& b, T t1, T t2);
    void add_at_offset(BaseMatrixT& b, int64_t col_offset);
    void clip(T t1, T t2);
    void down_clip(T t);
    void bigger_than_scalar(T t);
    void assign(BaseMatrixT& b);
    void assign_at_offset(BaseMatrixT& b, int64_t col_offset);
    void add_col_vector(BaseMatrixT& b);
    void add_row_vector(BaseMatrixT& b);
    void add_bias(BaseMatrixT& b, T scale);

    void mul_row_vector(BaseMatrixT& b);
    void div_row_vector(BaseMatrixT& b);
    void add_p2p(BaseMatrixT& b);
    void sub(BaseMatrixT& b);

    /*
     * this = this - b * t
     */
    void sub(BaseMatrixT& b, T t);

    /*
     * b = max(0, this)
     */
    void relu(BaseMatrixT& b);
    void relu_derivative(BaseMatrixT& b);

    /*
     * b = log(1.0 + exp(this))
     */
    void soft_relu(BaseMatrixT& b);
    void soft_relu_derivative(BaseMatrixT & b);

    void brelu(BaseMatrixT& b);
    void brelu_derivative(BaseMatrixT& b);

    /*
     *  b = this * this;
     */
    void square(BaseMatrixT& b);
    void square_derivative(BaseMatrixT& b);

    /*
     * b = tanh(this)
     */
    void tanh(BaseMatrixT& b);
    void tanh_derivative(BaseMatrixT& b);

    /*
     * b = t1 * tanh(t2 * this)
     */
    void scaled_tanh(BaseMatrixT& b,
                     T t1,
                     T t2);
    void scaled_tanh_derivative(BaseMatrixT& b,
                                T t1,
                                T t2);

    /*
     * b = 1.0f / this
     */
    void reciprocal(BaseMatrixT& b);
    void reciprocal_derivative(BaseMatrixT& b);

    void abs(BaseMatrixT& b);
    void abs_derivative(BaseMatrixT& b);

    /*
     * b = 1.0f / (1.0f + exp(-this))
     */
    void signmoid(BaseMatrixT& b);
    void signmoid_derivative(BaseMatrixT& b);

    void add_scalar(BaseMatrixT& b, T t);
    void sub_scalar(BaseMatrixT& b, T t);
    void mul_scalar(BaseMatrixT& b, T t);
    void div_scalar(BaseMatrixT& b, T t);
    void scalar_div(BaseMatrixT& b, T t);

    /*
     * this = 1.0f / sqrt(b)
     */
    void inv_sqrt(BaseMatrixT& b);

    /*
     * this = (b == t)
     */
    void is_equal_to(BaseMatrixT & b, T t);

    void soft_cross_entropy(BaseMatrixT& b, BaseMatrixT& c);
    void soft_cross_entropy_bp(BaseMatrixT& b, BaseMatrixT& c);
    void binary_label_cross_entropy(BaseMatrixT& b, BaseMatrixT& c);
    void binary_label_cross_entropy_bp(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * this = b + c
     */
    void add(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = b * t1 + c * t2
     */
    void add(BaseMatrixT& b,
             T t1,
             BaseMatrixT& c,
             T t2);
    /*
     * this = b - c
     */
    void sub(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = b * t1 - c * t2
     */
    void sub(BaseMatrixT& b,
             T t1,
             BaseMatrixT& c,
             T t2);
    /*
     * this = this + b + c
     */
    void add2(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = this * t1 + b * t2 + c * t3
     */
    void add2(BaseMatrixT& b,
              BaseMatrixT& c,
              T t1,
              T t2,
              T t3);
    /*
     * this = b * t1 + c * t2 + d * t3
     */
    void add3(BaseMatrixT& b,
              BaseMatrixT& c,
              BaseMatrixT& d,
              T t1,
              T t2,
              T t3);

    /*
     * mom = momentum * mom - learning_rate * (grad + decay_rate * this)
     * this += mom
     */
    void sgd_update(BaseMatrixT& grad,//grad
                    BaseMatrixT& mom,// mom
                    T learning_rate,// learning rate
                    T momentum,// momentum
                    T decay_rate); // decay rate
    /*
     * c = t2 * c - t1 * d * (b + t3 * this)
     * this += c
     */
    void sgd_update(BaseMatrixT& grad,//grad
                    BaseMatrixT& mom,//mom
                    BaseMatrixT& lr,//lr
                    T learning_rate,//learning rate
                    T momentum,//momentum
                    T decay_rate);//decay rate
    void apply_l1(T learning_rate, T decay_rate);
    void apply_l1(BaseMatrixT& lr,
                  T learning_rate,
                  T decay_rate);
    void apply_l2(T learning_rate, T decay_rate);
    void apply_l2(BaseMatrixT& lr,
                  T learning_rate,
                  T decay_rate);

    /*
     * this *= b
     */
    void dot_mul(BaseMatrixT& b);
    /*
     * this = b * c
     */
    void dot_mul(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * this = b / c
     */
    void dot_div(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = (b + t1) / (c + t2)
     */
    void dot_div(BaseMatrixT& b,
                 BaseMatrixT& c,
                 T t1,
                 T t2);

    /*
     * this = log(1 + exp(b - c)) - d * (b - c)
     */
    void rank_loss(BaseMatrixT& b,
                   BaseMatrixT& c,
                   BaseMatrixT& d);

    void rank_loss_bp(BaseMatrixT& b,
                   BaseMatrixT& c,
                   BaseMatrixT& d);

    /*
     * this = log(1 + exp(b)) - c * b
     */
    void logistic_regression_loss(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this += exp(b)/(1+exp(b)) - c
     */
    void logistic_regression_loss_bp(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * this = b > c ? 1.0 : 0.0
     */
    void bigger_than(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = ((b > c && d > 0.5) || (b < c && c < 0.5)) ? 1 : 0
     */
    void bigger_than(BaseMatrixT& b,
                     BaseMatrixT& c,
                     BaseMatrixT& d);

    void max(BaseMatrixT& b, BaseMatrixT& c);

    void binary_classification_error(size_t dest_col,
                                     BaseMatrixT& b,
                                     BaseMatrixT& c,
                                     BaseMatrixT& d,
                                     T t);
    void binary_classification_error2(size_t dest_col,
                                      BaseMatrixT& b,
                                      BaseMatrixT& c,
                                      BaseMatrixT& d,
                                      T t);
    /*
     * this = this * b * b
     */
    void dot_mul_square(BaseMatrixT& b);
    /*
     * this = b * c * c
     */
    void dot_mul_square(BaseMatrixT& b, BaseMatrixT& c);
    /*
     * this = this * this * b
     */
    void dot_square_mul(BaseMatrixT& b);

    /*
     * this = b * b * c * c
     */
    void dot_square_square(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * this = this * (t1 * b + t2 * c)^2
     */
    void dot_mul_square_sum(BaseMatrixT& b,
                            BaseMatrixT& c,
                            T t1,
                            T t2);
    /*
     * this = (t1 * b + t2 * c)^2
     */
    void dot_square_sum(BaseMatrixT& b,
                        BaseMatrixT& c,
                        T t1,
                        T t2);
    /*
     * this = this * (t1 * b + t2 * c)
     */
    void dot_mul_sum(BaseMatrixT& b,
                     BaseMatrixT& c,
                     T t1,
                     T t2);
    /*
     * this += sqr(t1 * b + t2 * c + t3 * d)
     */
    void add_square_sum(BaseMatrixT& b,
                        BaseMatrixT& c,
                        BaseMatrixT& d,
                        T t1,
                        T t2,
                        T t3);
    /*
     * this += t * sqr(b)
     */
    void add_square(BaseMatrixT& b, T t);
    /*
     * this = t1 * this + t2 * sqr(b)
     */
    void decay_add_square(BaseMatrixT& b,
                          T t1,
                          T t2);
    /*
     * this = t1 * this + t2 * sqr(b * c)
     */
    void decay_add_square_mul(BaseMatrixT& b,
                              BaseMatrixT& c,
                              T t1,
                              T t2);
    /*
     * this = 1 / (t1 * b + t2)
     */
    void reciprocal(BaseMatrixT& b,
                    T t1,
                    T t2);
    /*
     * this = 1 / (t1 * b + t2 * c + t3)
     */
    void reciprocal_sum(BaseMatrixT& b,
                        BaseMatrixT& c,
                        T t1,
                        T t2,
                        T t3);

    /*
     * b = this; this = 0
     */
    void copy_and_clear(BaseMatrixT& b);

    /*
     * this_row[dest_col] += dotprod(b_row, c_row)
     */
    void row_dot_mul(size_t dest_col,
                     BaseMatrixT& b,
                     BaseMatrixT& c);
    void row_dot_mul2(size_t dest_col,
                     BaseMatrixT& b,
                     BaseMatrixT& c);

    /*
     * this = t1 * this + t2 *  b * c
     */
    void add_dot_mul(BaseMatrixT& b,
                     BaseMatrixT& c,
                     T t1,
                     T t2);
    /*
     *  vector (one row matrix)
     *   for each row i, do:
     *      this_row += dot_mul(b_row_i, c_row_i)
     */
    void add_dot_mul_vmm(BaseMatrixT& b, BaseMatrixT& c);
    void add_dot_mul_vmm2(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * c is vector (one row matrix)
     * for each row i, do:
     *  this_row_i += dot_mul(b_row_i, c_row)
     */
    void add_dot_mul_mmv(BaseMatrixT& b, BaseMatrixT& c);
    void add_dot_mul_mmv2(BaseMatrixT& b, BaseMatrixT& c);

    /*
     * this_row = b_row * c_row[col]
     */
    void row_scale(size_t col,
                   BaseMatrixT& b,
                   BaseMatrixT& c);
    void row_scale2(size_t col,
                   BaseMatrixT& b,
                   BaseMatrixT& c);

    /*
     * this_col = b_col * c_col[row]
     */
    void col_scale(size_t row,
                   BaseMatrixT& b,
                   BaseMatrixT& c);

    /*
     * this_col += b_col * c_col[row]
     */
    void add_col_scale(size_t row,
                       BaseMatrixT& b,
                       BaseMatrixT& c);
    /*
     * this_row += b_row * c_row[col]
     */
    void add_row_scale(size_t col,
                       BaseMatrixT& b,
                       BaseMatrixT& c);
    /*
     * calculate the sum of each row of the matrix b.
     */
    void sum_rows(BaseMatrixT& b);
    /*
     * calculate the maximum value of each row of the matrix b.
     */
    void max_rows(BaseMatrixT& b);
    /*
     * calculate the minimum value of each row of the matrix b.
     */
    void min_rows(BaseMatrixT& b);
    /*
     * calculate the sum of each column of the matrix b.
     */
    void sum_cols(BaseMatrixT& b);
    void sum_cols(BaseMatrixT& b, T scale);
    /*
     * calculate the maximum value of each column of the matrix b.
     */
    void max_cols(BaseMatrixT& b);
    /*
     * calculate the minimum value of each column of the matrix b.
     */
    void min_cols(BaseMatrixT& b);

    /*
     * calculate the sum of each row of(b - c)^2.
     */
    void sum_of_squares(BaseMatrixT& b, BaseMatrixT& c);
    /*
     *  this_row = b_row + t * ones * c_row[col]
     */
    void row_add(size_t col,
                 BaseMatrixT& b,
                 BaseMatrixT& c,
                 T t);

    /*
     * this_row = pow(b_row, c_row[col])
     */
    void row_pow(size_t col,
                 BaseMatrixT& b,
                 BaseMatrixT& c);

    virtual bool is_sparse() const{
        return false;
    }
};//class BaseMatrixT

typedef BaseMatrixT<real> BaseMatrix;
typedef BaseMatrixT<int> IBaseMatrix;

}//namespace math
}//namespace ccml
#endif
