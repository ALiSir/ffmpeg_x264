
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_CHAR_H__
#define __TOOL_CHAR_H__


int unicode_to_utf8(unsigned short *in, int insize, unsigned char **out);

int utf8_to_unicode(unsigned char *in, unsigned short **out, int *outsize);


#endif

#if defined __cplusplus
}
#endif



