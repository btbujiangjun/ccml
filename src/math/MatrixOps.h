/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-26 16:29
* Last modified: 2016-09-26 16:29
* Filename: MatrixOps.h
* Description: 
**********************************************/
#ifndef CCML_MATH_MATRIX_OPS_H
#define CCML_MATH_MATRIX_OPS_H

namespace ccml{
namespace math{

#ifdef __NVCC__
#define HL_DEVICE __device__
#else
#define HL_DEVICE
#endif /* NVCC */


#define ONE_PARAMETER(name) \
    private:\
        const T t;\
    public:\
        name(const T s) : t(s){}

#define TWO_PARAMETER(name) \
    private:\
        const T t1;\
        const T t2;\
    public:\
        name(const T s1, T s2) : t1(s1), t2(s2) {}

#define THREE_PARAMETER(name) \
    private:\
        const T t1;\
        const T t2;\
        const T t3;\
    public:\
        name(const T s1, T s2, T s3) : t1(s1), t2(s2), t3(s3) {}

#define FOUR_PARAMETER(name) \
    private:\
        const T t1;\
        const T t2;\
        const T t3;\
        const T t4;\
    public:\
        name(const T s1, T s2, T s3, T s4) : t1(s1), t2(s2), t3(s3), t4(s4){}


#define DEFINE_MATRIX_UNARY_OP(name, op) \
    namespace unary{\
    template<class T>\
    class name{\
    public:\
        HL_DEVICE inline void gpu_operator(T& a){op;}\
        inline void cpu_operator(T& a){op;}\
    };\
    }

#define DEFINE_MATRIX_UNARY_PARAMETER_OP(name,PARA_MACRO, op) \
    namespace unary{\
    template<class T>\
    class name{\
    PARA_MACRO(name)\
    public:\
        HL_DEVICE inline void gpu_operatoe(T& a){op;}\
        inline void cpu_operator(T& a){op;}\
    };\
    }

#define DEFINE_MATRIX_BINARY_OP(name, op) \
    namespace binary{\
    template<class T>\
    class name {\
        HL_DEVICE inline void gpu_operator(T& a, T& b){op;}\
        inline void cpu_operator(T& a, T& b) {op;}\
    };\
    }

#define DEFINE_MATRIX_BINARY_PARAMETER_OP(name, PARA_MACRO, op) \
    namespace binary{\
    template<class T>\
    class name{\
    PARA_MACRO(name)\
    public:\
        HL_DEVICE inline void gpu_operator(T& a, T& b){op;}\
        inline void cpu_operator(T& a, T& b){op;}\
    };\
    }

#define DEFINE_MATRIX_TERNARY_OP(name, op) \
    namespace ternary{\
    template<class T>\
    class name{\
    public:\
        HL_DEVICE inline void gpu_operator(T& a, T& b, T& c){op;}\
        inline void cpu_operator(T& a, T& b, T& c) {op;}\
    };\
    }

#define DEFINE_MATRIX_TERNARY_PARAMETER_OP(name, PARA_MACRO, op) \
    namespace ternary {\
    template<class T>\
    class name{\
    private:\
        PARA_MACRO(name)\
    public:\
        HL_DEVICE inline void gpu_operator(T& a, T& b, T& c){op;}\
        inline void cpu_operator(T& a, T& b, T& c){op;}\
    };\
    }

#define DEFINE_MATRIX_QUATERNARY_OP(name, op) \
    namespace quaternary{\
    template<class T>\
    class name {\
    public:\
        HL_DEVICE inline void gpu_operator(T& a, T& b, T& c, T& d) {op;}\
        inline void cpu_operator(T& a, T& b, T& c, T& d){op;}\
    };\
    }

#define DEFINE_MATRIX_QUATERNARY_PARAMETER_OP(name, PARA_MACRO, op) \
    namespace quaternary{\
    template<class T>\
    class name{\
    private:\
        PARA_MACRO(name)\
    public:\
        HL_DEVICE inline void gpu_operator(T& a, T& b, T& c, T& d){op;}\
        inline void cpu_operator(T& a, T& b, T& c, T& d){op;}\
    };\
    }

}//namespace math
}//namespace ccml

#endif /*CCML_MATH_MATRIX_OPS_H*/
