#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_INTMATH_H
#define lan_AVUTIL_INTMATH_H

#include <stdint.h>
#include "lan_attributes.h"

extern const uint8_t lan_ff_log2_tab[256];


#ifndef lan_ff_log2
#define lan_ff_log2 lan_ff_log2_c
static lan_av_always_inline lan_av_const int lan_ff_log2_c(unsigned int v)
{
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += lan_ff_log2_tab[v];

    return n;
}
#endif


#endif /* lan_AVUTIL_INTMATH_H */

#if defined __cplusplus
}
#endif
