
#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVCODEC_MATHOPS_H
#define lan_AVCODEC_MATHOPS_H

#include "lan_attributes.h"
#include <stdint.h>

extern const uint8_t lan_ff_zigzag_direct[64];

#ifndef lan_sign_extend
static inline lan_av_const int lan_sign_extend(int val, unsigned bits)
{
    unsigned shift = 8 * sizeof(int) - bits;
    union { unsigned u; int s; } v = { (unsigned) val << shift };
    return v.s >> shift;
}
#endif

#ifndef lan_zero_extend
static inline lan_av_const unsigned lan_zero_extend(unsigned val, unsigned bits)
{
    return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}
#endif

#ifndef lan_NEG_SSR32
#define lan_NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#endif

#ifndef lan_NEG_USR32
#define lan_NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif


#endif /* lan_AVCODEC_MATHOPS_H */

#if defined __cplusplus
}
#endif
