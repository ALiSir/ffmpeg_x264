#if defined __cplusplus
extern "C"
{
#endif

#ifndef AVCODEC_GOLOMB_H
#define AVCODEC_GOLOMB_H

#include "lan_error.h"

extern const uint8_t lan_ff_golomb_vlc_len[512];
extern const uint8_t lan_ff_ue_golomb_vlc_code[512];
extern const  int8_t lan_ff_se_golomb_vlc_code[512];

/**
 * read unsigned exp golomb code.
 */
static inline int lan_get_ue_golomb(lan_GetBitContext *gb)
{
    unsigned int buf;

    lan_OPEN_READER(re, gb);
    lan_UPDATE_CACHE(re, gb);
    buf = lan_GET_CACHE(re, gb);

    if (buf >= (1 << 27)) {
        buf >>= 32 - 9;
        lan_LAST_SKIP_BITS(re, gb, lan_ff_golomb_vlc_len[buf]);
        lan_CLOSE_READER(re, gb);
        return lan_ff_ue_golomb_vlc_code[buf];
    } else {
        int log = 2 * lan_av_log2(buf) - 31;
        lan_LAST_SKIP_BITS(re, gb, 32 - log);
        lan_CLOSE_READER(re, gb);
//        if (CONFIG_FTRAPV && log < 0) {
		if (log < 0) {
  //          av_log(0, AV_LOG_ERROR, "Invalid UE golomb code\n");
            return lan_AVERROR_INVALIDDATA;
        }
        buf >>= log;
        buf--;

        return buf;
    }
}


/**
 * read unsigned exp golomb code, constraint to a max of 31.
 * the return value is undefined if the stored value exceeds 31.
 */
static inline int lan_get_ue_golomb_31(lan_GetBitContext *gb)
{
    unsigned int buf;

    lan_OPEN_READER(re, gb);
    lan_UPDATE_CACHE(re, gb);
    buf = lan_GET_CACHE(re, gb);

    buf >>= 32 - 9;
    lan_LAST_SKIP_BITS(re, gb, lan_ff_golomb_vlc_len[buf]);
    lan_CLOSE_READER(re, gb);

    return lan_ff_ue_golomb_vlc_code[buf];
}

/**
 * read signed exp golomb code.
 */
static inline int lan_get_se_golomb(lan_GetBitContext *gb)
{
    unsigned int buf;

    lan_OPEN_READER(re, gb);
    lan_UPDATE_CACHE(re, gb);
    buf = lan_GET_CACHE(re, gb);

    if (buf >= (1 << 27)) {
        buf >>= 32 - 9;
        lan_LAST_SKIP_BITS(re, gb, lan_ff_golomb_vlc_len[buf]);
        lan_CLOSE_READER(re, gb);

        return lan_ff_se_golomb_vlc_code[buf];
    } else {
        int log = lan_av_log2(buf);
        lan_LAST_SKIP_BITS(re, gb, 31 - log);
        lan_UPDATE_CACHE(re, gb);
        buf = lan_GET_CACHE(re, gb);

        buf >>= log;

        lan_LAST_SKIP_BITS(re, gb, 32 - log);
        lan_CLOSE_READER(re, gb);

        if (buf & 1)
            buf = -(buf >> 1);
        else
            buf = (buf >> 1);

        return buf;
    }
}

/**
 * Read an unsigned Exp-Golomb code in the range 0 to UINT32_MAX-1.
 */
static inline unsigned lan_get_ue_golomb_long(lan_GetBitContext *gb)
{
    unsigned buf, log;

    buf = lan_show_bits_long(gb, 32);
    log = 31 - lan_av_log2(buf);
    lan_skip_bits_long(gb, log);

    return lan_get_bits_long(gb, log + 1) - 1;
}


#endif /* AVCODEC_GOLOMB_H */

#if defined __cplusplus
}
#endif
