#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_RATIONAL_H
#define lan_AVUTIL_RATIONAL_H

/**
 * rational number numerator/denominator
 */
typedef struct lan_AVRational{
    int num; ///< numerator
    int den; ///< denominator
} lan_AVRational;

#endif /* lan_AVUTIL_RATIONAL_H */

#if defined __cplusplus
}
#endif
