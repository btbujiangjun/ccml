/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-09-21 17:50
* Last modified: 2016-09-21 17:50
* Filename: TypeDefs.h
* Description: 
**********************************************/

#ifndef CCML_UTILS_TYPEDEFS_H
#define CCML_UTILS_TYPEDEFS_H
namespace ccml{
namespace utils{
#ifdef CCML_TYPE_DOUBLE
    typedef double real;
#else
    typedef float real;
#endif
}//namespace utils
}//namespace ccml
#endif
