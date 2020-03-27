#if defined __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include "lan_h264.h"
#include "lan_h264data.h"
#include "lan_golomb.h"
#include "lan_mem.h"
#include "lan_imgutils.h"
#include "tool_log.h"

#define lan_MAX_LOG2_MAX_FRAME_NUM    (12 + 4)
#define lan_MIN_LOG2_MAX_FRAME_NUM    4

#ifdef WIN32

const uint8_t lan_ff_golomb_vlc_len[512]={
	19,17,15,15,13,13,13,13,11,11,11,11,11,11,11,11,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const uint8_t lan_ff_ue_golomb_vlc_code[512]={
	32,32,32,32,32,32,32,32,31,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


const int8_t lan_ff_se_golomb_vlc_code[512]={
	17, 17, 17, 17, 17, 17, 17, 17, 16, 17, 17, 17, 17, 17, 17, 17,  8, -8,  9, -9, 10,-10, 11,-11, 12,-12, 13,-13, 14,-14, 15,-15,
	4,  4,  4,  4, -4, -4, -4, -4,  5,  5,  5,  5, -5, -5, -5, -5,  6,  6,  6,  6, -6, -6, -6, -6,  7,  7,  7,  7, -7, -7, -7, -7,
	2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

const uint8_t lan_ff_log2_tab[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

#endif


static const lan_AVRational lan_pixel_aspect[17] = {
    {   0,  1 },
    {   1,  1 },
    {  12, 11 },
    {  10, 11 },
    {  16, 11 },
    {  40, 33 },
    {  24, 11 },
    {  20, 11 },
    {  32, 11 },
    {  80, 33 },
    {  18, 11 },
    {  15, 11 },
    {  64, 33 },
    { 160, 99 },
    {   4,  3 },
    {   3,  2 },
    {   2,  1 },
};


static const uint8_t lan_default_scaling8[2][64] = {
    {  6, 10, 13, 16, 18, 23, 25, 27,
      10, 11, 16, 18, 23, 25, 27, 29,
      13, 16, 18, 23, 25, 27, 29, 31,
      16, 18, 23, 25, 27, 29, 31, 33,
      18, 23, 25, 27, 29, 31, 33, 36,
      23, 25, 27, 29, 31, 33, 36, 38,
      25, 27, 29, 31, 33, 36, 38, 40,
      27, 29, 31, 33, 36, 38, 40, 42 },
    {  9, 13, 15, 17, 19, 21, 22, 24,
      13, 13, 17, 19, 21, 22, 24, 25,
      15, 17, 19, 21, 22, 24, 25, 27,
      17, 19, 21, 22, 24, 25, 27, 28,
      19, 21, 22, 24, 25, 27, 28, 30,
      21, 22, 24, 25, 27, 28, 30, 32,
      22, 24, 25, 27, 28, 30, 32, 33,
      24, 25, 27, 28, 30, 32, 33, 35 }
};

#define lan_QP(qP, depth) ((qP) + 6 * ((depth) - 8))

#define lan_CHROMA_lan_QP_TABLE_END(d)                                          \
    lan_QP(0,  d), lan_QP(1,  d), lan_QP(2,  d), lan_QP(3,  d), lan_QP(4,  d), lan_QP(5,  d),   \
    lan_QP(6,  d), lan_QP(7,  d), lan_QP(8,  d), lan_QP(9,  d), lan_QP(10, d), lan_QP(11, d),   \
    lan_QP(12, d), lan_QP(13, d), lan_QP(14, d), lan_QP(15, d), lan_QP(16, d), lan_QP(17, d),   \
    lan_QP(18, d), lan_QP(19, d), lan_QP(20, d), lan_QP(21, d), lan_QP(22, d), lan_QP(23, d),   \
    lan_QP(24, d), lan_QP(25, d), lan_QP(26, d), lan_QP(27, d), lan_QP(28, d), lan_QP(29, d),   \
    lan_QP(29, d), lan_QP(30, d), lan_QP(31, d), lan_QP(32, d), lan_QP(32, d), lan_QP(33, d),   \
    lan_QP(34, d), lan_QP(34, d), lan_QP(35, d), lan_QP(35, d), lan_QP(36, d), lan_QP(36, d),   \
    lan_QP(37, d), lan_QP(37, d), lan_QP(37, d), lan_QP(38, d), lan_QP(38, d), lan_QP(38, d),   \
    lan_QP(39, d), lan_QP(39, d), lan_QP(39, d), lan_QP(39, d)

const uint8_t lan_ff_h264_chroma_qp[7][lan_lan_QP_MAX_NUM + 1] = {
    { lan_CHROMA_lan_QP_TABLE_END(8) },
    { 0, 1, 2, 3, 4, 5,
      lan_CHROMA_lan_QP_TABLE_END(9) },
    { 0, 1, 2, 3,  4,  5,
      6, 7, 8, 9, 10, 11,
      lan_CHROMA_lan_QP_TABLE_END(10) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      lan_CHROMA_lan_QP_TABLE_END(11) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
      lan_CHROMA_lan_QP_TABLE_END(12) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
      24,25,26,27, 28, 29,
      lan_CHROMA_lan_QP_TABLE_END(13) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
      24,25,26,27, 28, 29,
      30,31,32,33, 34, 35,
      lan_CHROMA_lan_QP_TABLE_END(14) },
};

static const uint8_t lan_default_scaling4[2][16] = {
    {  6, 13, 20, 28, 13, 20, 28, 32,
      20, 28, 32, 37, 28, 32, 37, 42 },
    { 10, 14, 20, 24, 14, 20, 24, 27,
      20, 24, 27, 30, 24, 27, 30, 34 }
};


static void lan_decode_scaling_list(lan_H264Context *h, uint8_t *factors, int size,
                                const uint8_t *jvt_list,
                                const uint8_t *fallback_list)
{
    int i, last = 8, next = 8;
    const uint8_t *scan = size == 16 ? lan_zigzag_scan : lan_ff_zigzag_direct;
    if (!lan_get_bits1(&h->gb)) /* matrix not written, we use the predicted one */
        memcpy(factors, fallback_list, size * sizeof(uint8_t));
    else
        for (i = 0; i < size; i++) {
            if (next)
                next = (last + lan_get_se_golomb(&h->gb)) & 0xff;
            if (!i && !next) { /* matrix not written, we use the preset one */
                memcpy(factors, jvt_list, size * sizeof(uint8_t));
                break;
            }
            last = factors[scan[i]] = next ? next : last;
        }
}


static void lan_decode_scaling_matrices(lan_H264Context *h, lan_SPS *sps,
                                    lan_PPS *pps, int is_sps,
                                    uint8_t(*scaling_matrix4)[16],
                                    uint8_t(*scaling_matrix8)[64])
{
    int fallback_sps = !is_sps && sps->scaling_matrix_present;
    const uint8_t *fallback[4] = {
        fallback_sps ? sps->scaling_matrix4[0] : lan_default_scaling4[0],
        fallback_sps ? sps->scaling_matrix4[3] : lan_default_scaling4[1],
        fallback_sps ? sps->scaling_matrix8[0] : lan_default_scaling8[0],
        fallback_sps ? sps->scaling_matrix8[3] : lan_default_scaling8[1]
    };
    if (lan_get_bits1(&h->gb)) {
        sps->scaling_matrix_present |= is_sps;
        lan_decode_scaling_list(h, scaling_matrix4[0], 16, lan_default_scaling4[0], fallback[0]);        // Intra, Y
        lan_decode_scaling_list(h, scaling_matrix4[1], 16, lan_default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
        lan_decode_scaling_list(h, scaling_matrix4[2], 16, lan_default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
        lan_decode_scaling_list(h, scaling_matrix4[3], 16, lan_default_scaling4[1], fallback[1]);        // Inter, Y
        lan_decode_scaling_list(h, scaling_matrix4[4], 16, lan_default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
        lan_decode_scaling_list(h, scaling_matrix4[5], 16, lan_default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
        if (is_sps || pps->transform_8x8_mode) {
            lan_decode_scaling_list(h, scaling_matrix8[0], 64, lan_default_scaling8[0], fallback[2]); // Intra, Y
            lan_decode_scaling_list(h, scaling_matrix8[3], 64, lan_default_scaling8[1], fallback[3]); // Inter, Y
            if (sps->chroma_format_idc == 3) {
                lan_decode_scaling_list(h, scaling_matrix8[1], 64, lan_default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
                lan_decode_scaling_list(h, scaling_matrix8[4], 64, lan_default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
                lan_decode_scaling_list(h, scaling_matrix8[2], 64, lan_default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
                lan_decode_scaling_list(h, scaling_matrix8[5], 64, lan_default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
            }
        }
    }
}

static inline int lan_decode_hrd_parameters(lan_H264Context *h, lan_SPS *sps)
{
    int cpb_count, i;
    cpb_count = lan_get_ue_golomb_31(&h->gb) + 1;

    if (cpb_count > 32U) {
 //       av_log(h->avctx, AV_LOG_ERROR, "cpb_count %d invalid\n", cpb_count);
        return lan_AVERROR_INVALIDDATA;
    }

    lan_get_bits(&h->gb, 4); /* bit_rate_scale */
    lan_get_bits(&h->gb, 4); /* cpb_size_scale */
    for (i = 0; i < cpb_count; i++) {
        lan_get_ue_golomb_long(&h->gb); /* bit_rate_value_minus1 */
        lan_get_ue_golomb_long(&h->gb); /* cpb_size_value_minus1 */
        lan_get_bits1(&h->gb);          /* cbr_flag */
    }
    sps->initial_cpb_removal_delay_length = lan_get_bits(&h->gb, 5) + 1;
    sps->cpb_removal_delay_length         = lan_get_bits(&h->gb, 5) + 1;
    sps->dpb_output_delay_length          = lan_get_bits(&h->gb, 5) + 1;
    sps->time_offset_length               = lan_get_bits(&h->gb, 5);
    sps->cpb_cnt                          = cpb_count;
    return 0;
}

static inline int lan_decode_vui_parameters(lan_H264Context *h, lan_SPS *sps)
{
    int aspect_ratio_info_present_flag;
    unsigned int aspect_ratio_idc;

    aspect_ratio_info_present_flag = lan_get_bits1(&h->gb);

    if (aspect_ratio_info_present_flag) {
        aspect_ratio_idc = lan_get_bits(&h->gb, 8);
        if (aspect_ratio_idc == lan_EXTENDED_SAR) {
            sps->sar.num = lan_get_bits(&h->gb, 16);
            sps->sar.den = lan_get_bits(&h->gb, 16);
        } else if (aspect_ratio_idc < lan_FF_ARRAY_ELEMS(lan_pixel_aspect)) {
            sps->sar = lan_pixel_aspect[aspect_ratio_idc];
        } else {
  //          av_log(h->avctx, AV_LOG_ERROR, "illegal aspect ratio\n");
            return lan_AVERROR_INVALIDDATA;
        }
    } else {
        sps->sar.num =
        sps->sar.den = 0;
    }

    if (lan_get_bits1(&h->gb))      /* overscan_info_present_flag */
        lan_get_bits1(&h->gb);      /* overscan_appropriate_flag */

    sps->video_signal_type_present_flag = lan_get_bits1(&h->gb);
    if (sps->video_signal_type_present_flag) {
        lan_get_bits(&h->gb, 3);                 /* video_format */
        sps->full_range = lan_get_bits1(&h->gb); /* video_full_range_flag */

        sps->colour_description_present_flag = lan_get_bits1(&h->gb);
        if (sps->colour_description_present_flag) {
            sps->color_primaries = (enum lan_AVColorPrimaries)lan_get_bits(&h->gb, 8); /* colour_primaries */
            sps->color_trc       = (enum lan_AVColorTransferCharacteristic)lan_get_bits(&h->gb, 8); /* transfer_characteristics */
            sps->colorspace      = (enum lan_AVColorSpace)lan_get_bits(&h->gb, 8); /* matrix_coefficients */
            if (sps->color_primaries >= AVCOL_PRI_NB)
                sps->color_primaries = AVCOL_PRI_UNSPECIFIED;
            if (sps->color_trc >= AVCOL_TRC_NB)
                sps->color_trc = AVCOL_TRC_UNSPECIFIED;
            if (sps->colorspace >= AVCOL_SPC_NB)
                sps->colorspace = AVCOL_SPC_UNSPECIFIED;
        }
    }

    /* chroma_location_info_present_flag */
    if (lan_get_bits1(&h->gb)) {
        /* chroma_sample_location_type_top_field */
 //       h->avctx->chroma_sample_location = lan_get_ue_golomb(&h->gb) + 1;
 		int avctx_chroma_sample_location = lan_get_ue_golomb(&h->gb) + 1;
		log_debug("avctx_chroma_sample_location(%d)", avctx_chroma_sample_location);
        lan_get_ue_golomb(&h->gb);  /* chroma_sample_location_type_bottom_field */
    }

    if (lan_show_bits1(&h->gb) && lan_get_bits_left(&h->gb) < 10) {
 //       av_log(h->avctx, AV_LOG_WARNING, "Truncated VUI\n");
        return 0;
    }

    sps->timing_info_present_flag = lan_get_bits1(&h->gb);
    if (sps->timing_info_present_flag) {
        sps->num_units_in_tick = lan_get_bits_long(&h->gb, 32);
        sps->time_scale        = lan_get_bits_long(&h->gb, 32);
        if (!sps->num_units_in_tick || !sps->time_scale) {
//            av_log(h->avctx, AV_LOG_ERROR,
//                   "time_scale/num_units_in_tick invalid or unsupported (%"PRIu32"/%"PRIu32")\n",
//                   sps->time_scale, sps->num_units_in_tick);
            return lan_AVERROR_INVALIDDATA;
        }
        sps->fixed_frame_rate_flag = lan_get_bits1(&h->gb);
    }

    sps->nal_hrd_parameters_present_flag = lan_get_bits1(&h->gb);
    if (sps->nal_hrd_parameters_present_flag)
        if (lan_decode_hrd_parameters(h, sps) < 0)
            return lan_AVERROR_INVALIDDATA;
    sps->vcl_hrd_parameters_present_flag = lan_get_bits1(&h->gb);
    if (sps->vcl_hrd_parameters_present_flag)
        if (lan_decode_hrd_parameters(h, sps) < 0)
            return lan_AVERROR_INVALIDDATA;
    if (sps->nal_hrd_parameters_present_flag ||
        sps->vcl_hrd_parameters_present_flag)
        lan_get_bits1(&h->gb);     /* low_delay_hrd_flag */
    sps->pic_struct_present_flag = lan_get_bits1(&h->gb);
    if (!lan_get_bits_left(&h->gb))
        return 0;
    sps->bitstream_restriction_flag = lan_get_bits1(&h->gb);
    if (sps->bitstream_restriction_flag) {
        lan_get_bits1(&h->gb);     /* motion_vectors_over_pic_boundaries_flag */
        lan_get_ue_golomb(&h->gb); /* max_bytes_per_pic_denom */
        lan_get_ue_golomb(&h->gb); /* max_bits_per_mb_denom */
        lan_get_ue_golomb(&h->gb); /* log2_max_mv_length_horizontal */
        lan_get_ue_golomb(&h->gb); /* log2_max_mv_length_vertical */
        sps->num_reorder_frames = lan_get_ue_golomb(&h->gb);
        lan_get_ue_golomb(&h->gb); /*max_dec_frame_buffering*/

        if (lan_get_bits_left(&h->gb) < 0) {
            sps->num_reorder_frames         = 0;
            sps->bitstream_restriction_flag = 0;
        }

        if (sps->num_reorder_frames > 16U
            /* max_dec_frame_buffering || max_dec_frame_buffering > 16 */) {
 //           av_log(h->avctx, AV_LOG_ERROR,
 //                  "Clipping illegal num_reorder_frames %d\n",
 //                  sps->num_reorder_frames);
            sps->num_reorder_frames = 16;
            return lan_AVERROR_INVALIDDATA;
        }
    }

    if (lan_get_bits_left(&h->gb) < 0) {
 //       av_log(h->avctx, AV_LOG_ERROR,
 //              "Overread VUI by %d bits\n", -lan_get_bits_left(&h->gb));
        return lan_AVERROR_INVALIDDATA;
    }

    return 0;
}


int lan_ff_h264_decode_seq_parameter_set(lan_H264Context *h, int* width, int* height)
{
    int profile_idc, level_idc, constraint_set_flags = 0;
    unsigned int sps_id;
    int i, log2_max_frame_num_minus4;
    lan_SPS *sps;

    profile_idc           = lan_get_bits(&h->gb, 8);
    constraint_set_flags |= lan_get_bits1(&h->gb) << 0;   // constraint_set0_flag
    constraint_set_flags |= lan_get_bits1(&h->gb) << 1;   // constraint_set1_flag
    constraint_set_flags |= lan_get_bits1(&h->gb) << 2;   // constraint_set2_flag
    constraint_set_flags |= lan_get_bits1(&h->gb) << 3;   // constraint_set3_flag
    constraint_set_flags |= lan_get_bits1(&h->gb) << 4;   // constraint_set4_flag
    constraint_set_flags |= lan_get_bits1(&h->gb) << 5;   // constraint_set5_flag
    lan_skip_bits(&h->gb, 2);                             // reserved_zero_2bits
    level_idc = lan_get_bits(&h->gb, 8);
    sps_id    = lan_get_ue_golomb_31(&h->gb);

    if (sps_id >= lan_MAX_lan_SPS_COUNT) {
//        av_log(h->avctx, AV_LOG_ERROR, "sps_id %u out of range\n", sps_id);
        return lan_AVERROR_INVALIDDATA;
    }
    sps = (lan_SPS *)lan_av_mallocz(sizeof(lan_SPS));
    if (!sps)
        return lan_AVERROR(ENOMEM);

    sps->sps_id               = sps_id;
    sps->time_offset_length   = 24;
    sps->profile_idc          = profile_idc;
    sps->constraint_set_flags = constraint_set_flags;
    sps->level_idc            = level_idc;
    sps->full_range           = -1;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;
    sps->colorspace = (enum lan_AVColorSpace)2; //AVCOL_SPC_UNSPECIFIED

    if (sps->profile_idc == 100 ||  // High profile
        sps->profile_idc == 110 ||  // High10 profile
        sps->profile_idc == 122 ||  // High422 profile
        sps->profile_idc == 244 ||  // High444 Predictive profile
        sps->profile_idc ==  44 ||  // Cavlc444 profile
        sps->profile_idc ==  83 ||  // Scalable Constrained High profile (SVC)
        sps->profile_idc ==  86 ||  // Scalable High Intra profile (SVC)
        sps->profile_idc == 118 ||  // Stereo High profile (MVC)
        sps->profile_idc == 128 ||  // Multiview High profile (MVC)
        sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
        sps->profile_idc == 144) {  // old High444 profile
        sps->chroma_format_idc = lan_get_ue_golomb_31(&h->gb);
        if (sps->chroma_format_idc > 3U) {
//            avpriv_request_sample(h->avctx, "chroma_format_idc %u",
//                                  sps->chroma_format_idc);
            goto fail;
        } else if (sps->chroma_format_idc == 3) {
            sps->residual_color_transform_flag = lan_get_bits1(&h->gb);
            if (sps->residual_color_transform_flag) {
//                av_log(h->avctx, AV_LOG_ERROR, "separate color planes are not supported\n");
                goto fail;
            }
        }
        sps->bit_depth_luma   = lan_get_ue_golomb(&h->gb) + 8;
        sps->bit_depth_chroma = lan_get_ue_golomb(&h->gb) + 8;
        if (sps->bit_depth_chroma != sps->bit_depth_luma) {
  //          avpriv_request_sample(h->avctx,
   //                               "Different chroma and luma bit depth");
            goto fail;
        }
        if (sps->bit_depth_luma > 14U || sps->bit_depth_chroma > 14U) {
  //          av_log(h->avctx, AV_LOG_ERROR, "illegal bit depth value (%d, %d)\n",
  //                 sps->bit_depth_luma, sps->bit_depth_chroma);
            goto fail;
        }
        sps->transform_bypass = lan_get_bits1(&h->gb);
        lan_decode_scaling_matrices(h, sps, NULL, 1,
                                sps->scaling_matrix4, sps->scaling_matrix8);
    } else {
        sps->chroma_format_idc = 1;
        sps->bit_depth_luma    = 8;
        sps->bit_depth_chroma  = 8;
    }

    log2_max_frame_num_minus4 = lan_get_ue_golomb(&h->gb);
    if (log2_max_frame_num_minus4 < lan_MIN_LOG2_MAX_FRAME_NUM - 4 ||
        log2_max_frame_num_minus4 > lan_MAX_LOG2_MAX_FRAME_NUM - 4) {
//        av_log(h->avctx, AV_LOG_ERROR,
//               "log2_max_frame_num_minus4 out of range (0-12): %d\n",
//               log2_max_frame_num_minus4);
        goto fail;
    }
    sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

    sps->poc_type = lan_get_ue_golomb_31(&h->gb);

    if (sps->poc_type == 0) { // FIXME #define
        unsigned t = lan_get_ue_golomb(&h->gb);
        if (t>12) {
 //           av_log(h->avctx, AV_LOG_ERROR, "log2_max_poc_lsb (%d) is out of range\n", t);
 //           goto fail;
        }
        sps->log2_max_poc_lsb = t + 4;
    } else if (sps->poc_type == 1) { // FIXME #define
        sps->delta_pic_order_always_zero_flag = lan_get_bits1(&h->gb);
        sps->offset_for_non_ref_pic           = lan_get_se_golomb(&h->gb);
        sps->offset_for_top_to_bottom_field   = lan_get_se_golomb(&h->gb);
        sps->poc_cycle_length                 = lan_get_ue_golomb(&h->gb);

        if ((unsigned)sps->poc_cycle_length >=
            lan_FF_ARRAY_ELEMS(sps->offset_for_ref_frame)) {
 //           av_log(h->avctx, AV_LOG_ERROR,
 //                  "poc_cycle_length overflow %d\n", sps->poc_cycle_length);
            goto fail;
        }

        for (i = 0; i < sps->poc_cycle_length; i++)
            sps->offset_for_ref_frame[i] = lan_get_se_golomb(&h->gb);
    } else if (sps->poc_type != 2) {
//        av_log(h->avctx, AV_LOG_ERROR, "illegal POC type %d\n", sps->poc_type);
        goto fail;
    }

    sps->ref_frame_count = lan_get_ue_golomb_31(&h->gb);
//    if (h->avctx->codec_tag == lan_MKTAG('S', 'M', 'V', '2'))
//        sps->ref_frame_count = lan_FFMAX(2, sps->ref_frame_count);
//    if (sps->ref_frame_count > lan_H264_MAX_PICTURE_COUNT - 2 ||
//        sps->ref_frame_count > 16U) {
//        av_log(h->avctx, AV_LOG_ERROR,
//               "too many reference frames %d\n", sps->ref_frame_count);
//        goto fail;
//    }
    sps->gaps_in_frame_num_allowed_flag = lan_get_bits1(&h->gb);
    sps->mb_width                       = lan_get_ue_golomb(&h->gb) + 1;
    sps->mb_height                      = lan_get_ue_golomb(&h->gb) + 1;
//    if ((unsigned)sps->mb_width  >= INT_MAX / 16 ||
//        (unsigned)sps->mb_height >= INT_MAX / 16 ||
//        lan_av_image_check_size(16 * sps->mb_width,
//                            16 * sps->mb_height, 0, h->avctx)) {
//        av_log(h->avctx, AV_LOG_ERROR, "mb_width/height overflow\n");
//        goto fail;
//    }

    sps->frame_mbs_only_flag = lan_get_bits1(&h->gb);
    if (!sps->frame_mbs_only_flag)
        sps->mb_aff = lan_get_bits1(&h->gb);
    else
        sps->mb_aff = 0;

    sps->direct_8x8_inference_flag = lan_get_bits1(&h->gb);

#ifndef lan_ALLOW_INTERLACE
//    if (sps->mb_aff)
//        av_log(h->avctx, AV_LOG_ERROR,
//               "MBAFF support not included; enable it at compile-time.\n");
#endif
    sps->crop = lan_get_bits1(&h->gb);
    if (sps->crop) {
        int crop_left   = lan_get_ue_golomb(&h->gb);
        int crop_right  = lan_get_ue_golomb(&h->gb);
        int crop_top    = lan_get_ue_golomb(&h->gb);
        int crop_bottom = lan_get_ue_golomb(&h->gb);
        int width  = 16 * sps->mb_width;
        int height = 16 * sps->mb_height * (2 - sps->frame_mbs_only_flag);
		log_debug("crop(%d.%d.%d.%d) resolution(%d.%d)", crop_left, crop_right, crop_top, crop_bottom, width, height);
		/*
        if (h->avctx->flags2 & lan_CODEC_FLAG2_IGNORE_CROP) {
 //           av_log(h->avctx, AV_LOG_DEBUG, "discarding sps cropping, original "
 //                                          "values are l:%d r:%d t:%d b:%d\n",
 //                  crop_left, crop_right, crop_top, crop_bottom);

            sps->crop_left   =
            sps->crop_right  =
            sps->crop_top    =
            sps->crop_bottom = 0;
        } else {
            int vsub   = (sps->chroma_format_idc == 1) ? 1 : 0;
            int hsub   = (sps->chroma_format_idc == 1 ||
                          sps->chroma_format_idc == 2) ? 1 : 0;
            int step_x = 1 << hsub;
            int step_y = (2 - sps->frame_mbs_only_flag) << vsub;

            if (crop_left & (0x1F >> (sps->bit_depth_luma > 8)) &&
                !(h->avctx->flags & lan_CODEC_FLAG_UNALIGNED)) {
                crop_left &= ~(0x1F >> (sps->bit_depth_luma > 8));
 //               av_log(h->avctx, AV_LOG_WARNING,
 //                      "Reducing left cropping to %d "
 //                      "chroma samples to preserve alignment.\n",
 //                      crop_left);
            }

            if (crop_left  > (unsigned)INT_MAX / 4 / step_x ||
                crop_right > (unsigned)INT_MAX / 4 / step_x ||
                crop_top   > (unsigned)INT_MAX / 4 / step_y ||
                crop_bottom> (unsigned)INT_MAX / 4 / step_y ||
                (crop_left + crop_right ) * step_x >= width ||
                (crop_top  + crop_bottom) * step_y >= height
            ) {
   //             av_log(h->avctx, AV_LOG_ERROR, "crop values invalid %d %d %d %d / %d %d\n", crop_left, crop_right, crop_top, crop_bottom, width, height);
                goto fail;
            }

            sps->crop_left   = crop_left   * step_x;
            sps->crop_right  = crop_right  * step_x;
            sps->crop_top    = crop_top    * step_y;
            sps->crop_bottom = crop_bottom * step_y;
        }
		*/
    } else {
        sps->crop_left   =
        sps->crop_right  =
        sps->crop_top    =
        sps->crop_bottom =
        sps->crop        = 0;
    }

    sps->vui_parameters_present_flag = lan_get_bits1(&h->gb);
    if (sps->vui_parameters_present_flag) {
        int ret = lan_decode_vui_parameters(h, sps);
        if (ret < 0)
            goto fail;
    }

    if (!sps->sar.den)
        sps->sar.den = 1;

 //   if (h->avctx->debug & lan_FF_DEBUG_PICT_INFO) {
/* 
        static const char csp[4][5] = { "Gray", "420", "422", "444" };
        log_debug("sps:%u profile:%d/%d poc:%d ref:%d %dx%d %s %s crop:%u/%u/%u/%u %s %s b%d reo:%d\n",
               sps_id, sps->profile_idc, sps->level_idc,
               sps->poc_type,
               sps->ref_frame_count,
               sps->mb_width, sps->mb_height,
               sps->frame_mbs_only_flag ? "FRM" : (sps->mb_aff ? "MB-AFF" : "PIC-AFF"),
               sps->direct_8x8_inference_flag ? "8B8" : "",
               sps->crop_left, sps->crop_right,
               sps->crop_top, sps->crop_bottom,
               sps->vui_parameters_present_flag ? "VUI" : "",
               csp[sps->chroma_format_idc],
               sps->bit_depth_luma,
               sps->bitstream_restriction_flag ? sps->num_reorder_frames : -1
               );
*/               
 //   }
    sps->new_lan = 1;
 	*width = sps->mb_width*16;
	*height = sps->mb_height*16;

//    lan_av_free(h->sps_buffers[sps_id]);
//    h->sps_buffers[sps_id] = sps;
	lan_av_free(sps);

    return 0;


fail: 
	*width = sps->mb_width*16;
	*height = sps->mb_height*16; 
	lan_av_free(sps);
    return -1;
}

static int lan_more_rbsp_data_in_pps(lan_H264Context *h, lan_PPS *pps)
{
    const lan_SPS *sps = h->sps_buffers[pps->sps_id];
    int profile_idc = sps->profile_idc;

    if ((profile_idc == 66 || profile_idc == 77 ||
         profile_idc == 88) && (sps->constraint_set_flags & 7)) {
//        av_log(h->avctx, AV_LOG_VERBOSE,
//               "Current profile doesn't provide more RBSP data in lan_PPS, skipping\n");
        return 0;
    }

    return 1;
}

static void lan_build_qp_table(lan_PPS *pps, int t, int index, const int depth)
{
    int i;
    const int max_qp = 51 + 6 * (depth - 8);
    for (i = 0; i < max_qp + 1; i++)
        pps->chroma_qp_table[t][i] =
            lan_ff_h264_chroma_qp[depth - 8][lan_av_clip(i + index, 0, max_qp)];
}

int lan_ff_h264_decode_picture_parameter_set(lan_H264Context *h, int bit_length)
{
    unsigned int pps_id = lan_get_ue_golomb(&h->gb);
    lan_PPS *pps;
    lan_SPS *sps;
    int qp_bd_offset;
    int bits_left;

    if (pps_id >= lan_MAX_lan_PPS_COUNT) {
 //       av_log(h->avctx, AV_LOG_ERROR, "pps_id %u out of range\n", pps_id);
        return lan_AVERROR_INVALIDDATA;
    }

    pps = (lan_PPS *)lan_av_mallocz(sizeof(lan_PPS));
    if (!pps)
        return lan_AVERROR(ENOMEM);
    pps->sps_id = lan_get_ue_golomb_31(&h->gb);
    if ((unsigned)pps->sps_id >= lan_MAX_lan_SPS_COUNT ||
        h->sps_buffers[pps->sps_id] == NULL) {
 //       av_log(h->avctx, AV_LOG_ERROR, "sps_id %u out of range\n", pps->sps_id);
        goto fail;
    }
    sps = h->sps_buffers[pps->sps_id];
    qp_bd_offset = 6 * (sps->bit_depth_luma - 8);
    if (sps->bit_depth_luma > 14) {
//        av_log(h->avctx, AV_LOG_ERROR,
//               "Invalid luma bit depth=%d\n",
 //              sps->bit_depth_luma);
        goto fail;
    } else if (sps->bit_depth_luma == 11 || sps->bit_depth_luma == 13) {
 //       av_log(h->avctx, AV_LOG_ERROR,
//               "Unimplemented luma bit depth=%d\n",
 //              sps->bit_depth_luma);
        goto fail;
    }

    pps->cabac             = lan_get_bits1(&h->gb);
    pps->pic_order_present = lan_get_bits1(&h->gb);
    pps->slice_group_count = lan_get_ue_golomb(&h->gb) + 1;
    if (pps->slice_group_count > 1) {
        pps->mb_slice_group_map_type = lan_get_ue_golomb(&h->gb);
 //       av_log(h->avctx, AV_LOG_ERROR, "FMO not supported\n");
        switch (pps->mb_slice_group_map_type) {
        case 0:
#if 0
    |       for (i = 0; i <= num_slice_groups_minus1; i++)  |   |      |
    |           run_length[i]                               |1  |ue(v) |
#endif
            break;
        case 2:
#if 0
    |       for (i = 0; i < num_slice_groups_minus1; i++) { |   |      |
    |           top_left_mb[i]                              |1  |ue(v) |
    |           bottom_right_mb[i]                          |1  |ue(v) |
    |       }                                               |   |      |
#endif
            break;
        case 3:
        case 4:
        case 5:
#if 0
    |       slice_group_change_direction_flag               |1  |u(1)  |
    |       slice_group_change_rate_minus1                  |1  |ue(v) |
#endif
            break;
        case 6:
#if 0
    |       slice_group_id_cnt_minus1                       |1  |ue(v) |
    |       for (i = 0; i <= slice_group_id_cnt_minus1; i++)|   |      |
    |           slice_group_id[i]                           |1  |u(v)  |
#endif
            break;
        }
    }
    pps->ref_count[0] = lan_get_ue_golomb(&h->gb) + 1;
    pps->ref_count[1] = lan_get_ue_golomb(&h->gb) + 1;
    if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
 //       av_log(h->avctx, AV_LOG_ERROR, "reference overflow (pps)\n");
        goto fail;
    }

    pps->weighted_pred                        = lan_get_bits1(&h->gb);
    pps->weighted_bipred_idc                  = lan_get_bits(&h->gb, 2);
    pps->init_qp                              = lan_get_se_golomb(&h->gb) + 26 + qp_bd_offset;
    pps->init_qs                              = lan_get_se_golomb(&h->gb) + 26 + qp_bd_offset;
    pps->chroma_qp_index_offset[0]            = lan_get_se_golomb(&h->gb);
    pps->deblocking_filter_parameters_present = lan_get_bits1(&h->gb);
    pps->constrained_intra_pred               = lan_get_bits1(&h->gb);
    pps->redundant_pic_cnt_present            = lan_get_bits1(&h->gb);

    pps->transform_8x8_mode = 0;
    // contents of sps/pps can change even if id doesn't, so reinit
    h->dequant_coeff_pps = -1;
    memcpy(pps->scaling_matrix4, h->sps_buffers[pps->sps_id]->scaling_matrix4,
           sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, h->sps_buffers[pps->sps_id]->scaling_matrix8,
           sizeof(pps->scaling_matrix8));

    bits_left = bit_length - lan_get_bits_count(&h->gb);
    if (bits_left > 0 && lan_more_rbsp_data_in_pps(h, pps)) {
        pps->transform_8x8_mode = lan_get_bits1(&h->gb);
        lan_decode_scaling_matrices(h, h->sps_buffers[pps->sps_id], pps, 0,
                                pps->scaling_matrix4, pps->scaling_matrix8);
        // second_chroma_qp_index_offset
        pps->chroma_qp_index_offset[1] = lan_get_se_golomb(&h->gb);
    } else {
        pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
    }

    lan_build_qp_table(pps, 0, pps->chroma_qp_index_offset[0], sps->bit_depth_luma);
    lan_build_qp_table(pps, 1, pps->chroma_qp_index_offset[1], sps->bit_depth_luma);
    if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
        pps->chroma_qp_diff = 1;

    if (h->avctx->debug & lan_FF_DEBUG_PICT_INFO) {
        log_debug("pps:%u sps:%u %s slice_groups:%d ref:%u/%u %s qp:%d/%d/%d/%d %s %s %s %s\n",
               pps_id, pps->sps_id,
               pps->cabac ? "CABAC" : "CAVLC",
               pps->slice_group_count,
               pps->ref_count[0], pps->ref_count[1],
               pps->weighted_pred ? "weighted" : "",
               pps->init_qp, pps->init_qs, pps->chroma_qp_index_offset[0], pps->chroma_qp_index_offset[1],
               pps->deblocking_filter_parameters_present ? "LPAR" : "",
               pps->constrained_intra_pred ? "CONSTR" : "",
               pps->redundant_pic_cnt_present ? "REDU" : "",
               pps->transform_8x8_mode ? "8x8DCT" : "");
    }

    lan_av_free(h->pps_buffers[pps_id]);
    h->pps_buffers[pps_id] = pps;
    return 0;

fail:
    lan_av_free(pps);
    return -1;
}

#if defined __cplusplus
}
#endif

