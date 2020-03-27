#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_IMGUTILS_H
#define lan_AVUTIL_IMGUTILS_H


/**
 * Check if the given dimension of an image is valid, meaning that all
 * bytes of the image can be addressed with a signed int.
 *
 * @param w the width of the picture
 * @param h the height of the picture
 * @param log_offset the offset to sum to the log level for logging with log_ctx
 * @param log_ctx the parent logging context, it may be NULL
 * @return >= 0 if valid, a negative error code otherwise
 */
int lan_av_image_check_size(unsigned int w, unsigned int h, int log_offset, void *log_ctx);

#endif /* lan_AVUTIL_IMGUTILS_H */

#if defined __cplusplus
}
#endif

