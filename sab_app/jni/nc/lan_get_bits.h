
#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVCODEC_GET_BITS_H
#define lan_AVCODEC_GET_BITS_H

#include <stdint.h>
#include <limits.h>
#include "lan_avassert.h"
#include "lan_mathops.h"
#include "lan_common.h"
#include "lan_error.h"

typedef struct lan_GetBitContext {
    const uint8_t *buffer, *buffer_end;
    int index;
    int size_in_bits;
    int size_in_bits_plus8;
} lan_GetBitContext;


#define lan_OPEN_READER(name, gb)                   \
    unsigned int name ## _index = (gb)->index;  \
    unsigned int lan_av_unused name ## _cache;		\
    name ## _cache = 0;


#ifndef lan_AV_RL64
#   define lan_AV_RL64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[7] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[6] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[0])
#endif

#ifndef lan_AV_RB64
#define lan_AV_RB64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[0] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[1] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[6] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[7])
#endif

#ifndef lan_AV_RL32
#define lan_AV_RL32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[3] << 24) |    \
               (((const uint8_t*)(x))[2] << 16) |    \
               (((const uint8_t*)(x))[1] <<  8) |    \
                ((const uint8_t*)(x))[0])
#endif

#ifndef lan_AV_RB32
#define lan_AV_RB32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])
#endif



//x32 / x64
#ifdef lan_LONG_BITSTREAM_READER

#define lan_lan_UPDATE_CACHE_LE(name, gb) name ## _cache = \
      lan_AV_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

#define lan_lan_UPDATE_CACHE_BE(name, gb) name ## _cache = \
      lan_AV_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))

#else

#define lan_lan_UPDATE_CACHE_LE(name, gb) name ## _cache = \
      lan_AV_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

#define lan_lan_UPDATE_CACHE_BE(name, gb) name ## _cache = \
      lan_AV_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)

#endif

//#define lan_BITSTREAM_READER_LE

//low end / big end
#ifdef lan_BITSTREAM_READER_LE

#define lan_UPDATE_CACHE(name, gb) lan_lan_UPDATE_CACHE_LE(name, gb)

#define lan_SKIP_CACHE(name, gb, num) name ## _cache >>= (num)

#else

#define lan_UPDATE_CACHE(name, gb) lan_lan_UPDATE_CACHE_BE(name, gb)

#define lan_SKIP_CACHE(name, gb, num) name ## _cache <<= (num)

#endif

#define lan_lan_SHOW_UBITS_LE(name, gb, num) lan_zero_extend(name ## _cache, num)
#define lan_lan_SHOW_SBITS_LE(name, gb, num) lan_sign_extend(name ## _cache, num)

#define lan_lan_SHOW_UBITS_BE(name, gb, num) lan_NEG_USR32(name ## _cache, num)
#define lan_lan_SHOW_SBITS_BE(name, gb, num) lan_NEG_SSR32(name ## _cache, num)

#ifdef lan_BITSTREAM_READER_LE
#define lan_SHOW_UBITS(name, gb, num) lan_lan_SHOW_UBITS_LE(name, gb, num)
#define lan_SHOW_SBITS(name, gb, num) lan_lan_SHOW_SBITS_LE(name, gb, num)
#else
#define lan_SHOW_UBITS(name, gb, num) lan_lan_SHOW_UBITS_BE(name, gb, num)
#define lan_SHOW_SBITS(name, gb, num) lan_lan_SHOW_SBITS_BE(name, gb, num)
#endif

#define lan_SKIP_COUNTER(name, gb, num) name ## _index += (num)

#define lan_LAST_SKIP_BITS(name, gb, num) lan_SKIP_COUNTER(name, gb, num)

#define lan_CLOSE_READER(name, gb) (gb)->index = name ## _index

#define lan_GET_CACHE(name, gb) ((uint32_t) name ## _cache)


/**
 * Read 1-25 bits.
 */
static inline unsigned int lan_get_bits(lan_GetBitContext *s, int n)
{
    register int tmp;
    lan_OPEN_READER(re, s);
    lan_av_assert2(n>0 && n<=25);
    lan_UPDATE_CACHE(re, s);
    tmp = lan_SHOW_UBITS(re, s, n);
    lan_LAST_SKIP_BITS(re, s, n);
    lan_CLOSE_READER(re, s);
    return tmp;
}

static inline unsigned int lan_get_bits1(lan_GetBitContext *s)
{
    unsigned int index = s->index;
    uint8_t result     = s->buffer[index >> 3];
#ifdef lan_BITSTREAM_READER_LE
    result >>= index & 7;
    result  &= 1;
#else
    result <<= index & 7;
    result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
    if (s->index < s->size_in_bits_plus8)
#endif
        index++;
    s->index = index;

    return result;
}

static inline void lan_skip_bits(lan_GetBitContext *s, int n)
{
    lan_OPEN_READER(re, s);
    lan_LAST_SKIP_BITS(re, s, n);
    lan_CLOSE_READER(re, s);
}

/**
 * Show 1-25 bits.
 */
static inline unsigned int lan_show_bits(lan_GetBitContext *s, int n)
{
    register int tmp;
    lan_OPEN_READER(re, s);
    lan_av_assert2(n>0 && n<=25);
    lan_UPDATE_CACHE(re, s);
    tmp = lan_SHOW_UBITS(re, s, n);
    return tmp;
}


static inline unsigned int lan_show_bits1(lan_GetBitContext *s)
{
    return lan_show_bits(s, 1);
}

static inline int lan_get_bits_count(const lan_GetBitContext *s)
{
    return s->index;
}

static inline int lan_get_bits_left(lan_GetBitContext *gb)
{
    return gb->size_in_bits - lan_get_bits_count(gb);
}

#ifdef lan_LONG_BITSTREAM_READER
#   define MIN_CACHE_BITS 32
#else
#   define MIN_CACHE_BITS 25
#endif


/**
 * Read 0-32 bits.
 */
static inline unsigned int lan_get_bits_long(lan_GetBitContext *s, int n)
{
    if (!n) {
        return 0;
    } else if (n <= MIN_CACHE_BITS) {
        return lan_get_bits(s, n);
    } else {
#ifdef lan_BITSTREAM_READER_LE
        unsigned ret = lan_get_bits(s, 16);
        return ret | (lan_get_bits(s, n - 16) << 16);
#else
        unsigned ret = lan_get_bits(s, 16) << (n - 16);
        return ret | lan_get_bits(s, n - 16);
#endif
    }
}

/**
 * Show 0-32 bits.
 */
static inline unsigned int lan_show_bits_long(lan_GetBitContext *s, int n)
{
    if (n <= MIN_CACHE_BITS) {
        return lan_show_bits(s, n);
    } else {
        lan_GetBitContext gb = *s;
        return lan_get_bits_long(&gb, n);
    }
}

static inline void lan_skip_bits_long(lan_GetBitContext *s, int n)
{
#if UNCHECKED_BITSTREAM_READER
    s->index += n;
#else
    s->index += lan_av_clip(n, -s->index, s->size_in_bits_plus8 - s->index);
#endif
}

/**
 * Initialize lan_GetBitContext.
 * @param buffer bitstream buffer, must be FF_INPUT_BUFFER_PADDING_SIZE bytes
 *        larger than the actual read bits because some optimized bitstream
 *        readers read 32 or 64 bit at once and could read over the end
 * @param bit_size the size of the buffer in bits
 * @return 0 on success, lan_AVERROR_INVALIDDATA if the buffer_size would overflow.
 */
static inline int lan_init_lan_get_bits(lan_GetBitContext *s, const uint8_t *buffer,
                                int bit_size)
{
    int buffer_size;
    int ret = 0;

    if (bit_size >= INT_MAX - 7 || bit_size < 0 || !buffer) {
        buffer_size = bit_size = 0;
        buffer      = NULL;
        ret         = lan_AVERROR_INVALIDDATA;
    }

    buffer_size = (bit_size + 7) >> 3;

    s->buffer             = buffer;
    s->size_in_bits       = bit_size;
    s->size_in_bits_plus8 = bit_size + 8;
    s->buffer_end         = buffer + buffer_size;
    s->index              = 0;

    return ret;
}

#endif /* lan_AVCODEC_GET_BITS_H */

#if defined __cplusplus
}
#endif

