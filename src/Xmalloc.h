#ifndef __XMALLOC_H__
#define __XMALLOC_H__

#include <stddef.h>

void* Xmalloc(size_t Size);
void* Xrealloc(void* Ptr, size_t Size);
void* Xcalloc(size_t Num, size_t Size);

#endif
