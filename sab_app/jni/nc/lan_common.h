#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_COMMON_H
#define lan_AVUTIL_COMMON_H


#ifndef lan_av_log2
lan_av_const int lan_av_log2(unsigned v);
#endif

#define lan_FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define lan_FFMAX(a,b) ((a) > (b) ? (a) : (b))

/**
 * Clip a signed integer value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static lan_av_always_inline lan_av_const int lan_lan_av_clip_c(int a, int amin, int amax)
{
//#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
//    if (amin > amax) abort();
//#endif
    if      (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

#ifndef lan_av_clip
#   define lan_av_clip          lan_lan_av_clip_c
#endif


#endif /* lan_AVUTIL_COMMON_H */

#if defined __cplusplus
}
#endif
