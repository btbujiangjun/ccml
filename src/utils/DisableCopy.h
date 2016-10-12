/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-10-11 16:37
* Last modified: 2016-10-11 16:37
* Filename: DisableCopy.h
* Description: 
**********************************************/

#pragma once

#define DISABLE_COPY(CLASS_NAME)\
    CLASS_NAME(CLASS_NAME &&) = delete; \
    CLASS_NAME(const CLASS_NAME& other) = delete; \
    CLASS_NAME& operator=(const CLASS_NAME& other) = delete
