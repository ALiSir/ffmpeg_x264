#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_MEM_H
#define lan_AVUTIL_MEM_H

/**
 * Free a memory block which has been allocated with av_malloc(z)() or
 * av_realloc().
 * @param ptr Pointer to the memory block which should be freed.
 * @note ptr = NULL is explicitly allowed.
 * @note It is recommended that you use lan_av_freep() instead.
 * @see lan_av_freep()
 */
void lan_av_free(void *ptr);


/**
 * Allocate a block of size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU) and
 * zero all the bytes of the block.
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if it cannot be allocated.
 * @see av_malloc()
 */
void *lan_av_mallocz(size_t size) ;

#endif /* lan_AVUTIL_MEM_H */


#if defined __cplusplus
}
#endif
