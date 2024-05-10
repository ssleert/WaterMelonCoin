#include "Xmalloc.h"
#include <malloc.h>
#include "Log.h"

void* Xmalloc(size_t Size) {
  void* Ptr = malloc(Size);

  if (Ptr == NULL) {
    LogFatal("allocation failure");
  }

  return Ptr;
}

void* Xrealloc(void* Ptr, size_t Size) {
  void* PtrNew = realloc(Ptr, Size);

  if (PtrNew == NULL) {
    LogFatal("reallocation failure");
  }

  return PtrNew;
}

void* Xcalloc(size_t Num, size_t Size) {
  void* Ptr = calloc(Num, Size);

  if (Ptr == NULL) {
    LogFatal("allocation failure");
  }

  return Ptr;
}
