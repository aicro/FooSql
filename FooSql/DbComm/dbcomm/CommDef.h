#ifndef COMMDEF_H_
#define COMMDEF_H_

// Common header
// Unmacro it if you do not need debug info
#define NDEBUG
#include <assert.h>

#include "tool/ArrayDeleter.h"

// ACTION TYPE typedef
typedef const int ActionType_C;
typedef int       ActionType;

// CHARACTER SET TYPE typedef
typedef const int CharacterSetType_C;
typedef int       CharacterSetType;

// Write log to STANDARD ERROR
#define WRITE_LOG_TO_ERR(x) cerr << __FILE__ << ":" << __LINE__ << " : " << (x) << endl;

#endif
