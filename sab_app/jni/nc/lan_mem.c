#if defined __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include "lan_mem.h"


void *lan_av_mallocz(size_t size)
{
    void *ptr = malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void lan_av_free(void *ptr)
{
#if lan_CONFIG_MEMALIGN_HACK
    if (ptr) {
        int v= ((char *)ptr)[-1];
        lan_av_assert0(v>0 && v<=ALIGN);
        free((char *)ptr - v);
    }
#elif lan_HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

#if defined __cplusplus
}
#endif

