#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVCODEC_H264DATA_H
#define lan_AVCODEC_H264DATA_H

static const uint8_t lan_zigzag_scan[16+1] = {
    0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
    1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
    1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
    3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};


#endif /* lan_AVCODEC_H264DATA_H */

#if defined __cplusplus
}
#endif
