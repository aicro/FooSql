/// @file ArrayDeleter.h
/// @brief The file has defined an array deleter.

/// @author Aicro Ai
/// @date 2014/9/19

#ifndef ARRAY_DELETER_H_
#define ARRAY_DELETER_H_

// Delete Function Object
template<class T>
struct Array_Deleter
{   
    void operator()(T* p){ delete [] p; }
};

#endif