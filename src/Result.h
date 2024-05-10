#ifndef __RESULT_H__
#define __RESULT_H__

#include "Error.h"

#define Result(T) \
  struct {        \
    T     Res;    \
    Error Err;    \
  }

typedef char None;

#endif
