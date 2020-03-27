#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_AVASSERT_H
#define lan_AVUTIL_AVASSERT_H

#include <stdlib.h>


/**
 * assert() equivalent, that is always enabled.
 */
#define lan_av_assert0(cond) do {                                           \
    if (!(cond)) {                                                      \
        abort();                                                        \
    }                                                                   \
} while (0)


#define lan_av_assert2(cond) lan_av_assert0(cond)

#endif /* lan_AVUTIL_AVASSERT_H */

#if defined __cplusplus
}
#endif
