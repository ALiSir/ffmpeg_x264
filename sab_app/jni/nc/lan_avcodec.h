
#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVCODEC_AVCODEC_H
#define lan_AVCODEC_AVCODEC_H

#include "lan_pixfmt.h"

#define lan_CODEC_FLAG_UNALIGNED 0x0001

#define lan_CODEC_FLAG2_IGNORE_CROP   0x00010000 ///< Discard cropping information from lan_SPS.

#define lan_FF_DEBUG_PICT_INFO   1

/**
 * main external API structure.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * Please use AVOptions (av_opt* / av_set/get*()) to access these fields from user
 * applications.
 * sizeof(lan_AVCodecContext) must not be used outside libav*.
 */
typedef struct lan_AVCodecContext 
{
    /**
     * fourcc (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
     * This is used to work around some encoder bugs.
     * A demuxer should set this to what is stored in the field used to identify the codec.
     * If there are multiple such fields in a container then the demuxer should choose the one
     * which maximizes the information about the used codec.
     * If the codec tag field in a container is larger than 32 bits then the demuxer should
     * remap the longer ID to 32 bits with a table or other structure. Alternatively a new
     * extra_codec_tag + size could be added but for this a clear advantage must be demonstrated
     * first.
     * - encoding: Set by user, if not then the default based on codec_id will be used.
     * - decoding: Set by user, will be converted to uppercase by libavcodec during init.
     */
    unsigned int codec_tag;

    /**
     * CODEC_FLAG_*.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int flags;

    /**
     * debug
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int debug;	
	
	/**
     * CODEC_FLAG2_*
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int flags2;

	/**
     * This defines the location of chroma samples.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum lan_AVChromaLocation chroma_sample_location;
}lan_AVCodecContext;



#endif /* lan_AVCODEC_AVCODEC_H */

#if defined __cplusplus
}
#endif
