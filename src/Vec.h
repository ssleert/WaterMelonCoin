#ifndef __VEC_H__
#define __VEC_H__

#define USE_XMALLOC

#ifdef USE_XMALLOC 
#  include "Xmalloc.h"
#  define VecCallocImpl Xcalloc
#  define VecReallocImpl Xrealloc
#endif

#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>

#ifndef VecCallocImpl
#  define VecCallocImpl calloc
#endif

#ifndef VecReallocImpl
#  define VecReallocImpl realloc
#endif

#ifndef VecFreeImpl
#  define VecFreeImpl free
#endif

#define Vec(T)                                                      \
  struct {                                                          \
    size_t Len;                                                     \
    size_t Cap;                                                     \
    T*     Data;                                                    \
  }


#define VecGetDataElementSizeOfImpl(Self) sizeof(*((Self)->Data))
#define VecGetDataElementSizeOf(Self) VecGetDataElementSizeOfImpl(Self)

#define NewVec(T, Capacity) {                                       \
  .Len = 0,                                                         \
  .Cap = (Capacity),                                                \
  .Data = VecCallocImpl((Capacity), sizeof(T)),                     \
}

#define FreeVec(Self) do {                                          \
  if ((Self)->Data != NULL) VecFreeImpl((Self)->Data);              \
} while (false)

#define VecPush(Self, Obj) do {                                     \
  if ((Self)->Data == NULL) {                                       \
    (Self)->Data = VecCallocImpl(2, VecGetDataElementSizeOf(Self)); \
    assert((Self)->Data);                                           \
    (Self)->Len = 0;                                                \
    (Self)->Cap = 2;                                                \
  }                                                                 \
                                                                    \
  if ((Self)->Len == (Self)->Cap) {                                 \
    (Self)->Cap = (Self)->Cap * 2;                                  \
    (Self)->Data = VecReallocImpl((Self)->Data, (Self)->Cap * VecGetDataElementSizeOf(Self)); \
    assert((Self)->Data);                                           \
  }                                                                 \
                                                                    \
  (Self)->Data[(Self)->Len] = (Obj);                                \
  (Self)->Len++;                                                    \
} while (false)

#define VecShrink(Self) do {                                        \
  (Self)->Data = VecReallocImpl((Self)->Data, (Self)->Len * VecGetDataElementSizeOf(Self)); \
  assert((Self)->Data);                                             \
  (Self)->Cap = (Self)->Len;                                        \
} while (false)


#define VecReset(Self) do {                                         \
  memset((Self)->Data, 0, (Self)->Len * VecGetDataElementSizeOf(Self)); \
  (Self)->Len = 0;                                                  \
} while (false)


__attribute__((unused)) static int __VecRandCmp(const void* a, const void* b) {
  (void)a; (void)b;
  return rand() % 2 ? +1 : -1;
}

#define VecShuffle(Self) do {                                      \
  if ((Self)->Len > 1) {                                           \
    qsort(                                                         \
      (Self)->Data,                                                \
      (Self)->Len,                                                 \
      VecGetDataElementSizeOf(Self),                               \
      &__VecRandCmp                                                \
    );                                                             \
  }                                                                \
} while (false)

#endif

