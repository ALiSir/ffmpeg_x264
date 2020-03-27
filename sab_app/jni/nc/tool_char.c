



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <memory.h>
#include "tool_char.h"


int unicode_to_utf8(unsigned short *in, int insize, unsigned char **out)
{
    int i = 0;
    int outsize = 0;
    int charscount = 0;
    unsigned char *result = NULL;
    unsigned char *tmp = NULL;

    charscount = insize / sizeof(unsigned short);
    result = (unsigned char *)malloc(charscount * 3 + 1);
    memset(result, 0, charscount * 3 + 1);
    tmp = result;

    for (i = 0; i < charscount; i++)
    {
        unsigned short unicode = in[i];
        
        if (unicode >= 0x0000 && unicode <= 0x007f)
        {
            *tmp = (unsigned char)unicode;
            tmp += 1;
            outsize += 1;
        }
        else if (unicode >= 0x0080 && unicode <= 0x07ff)
        {
            *tmp = 0xc0 | (unicode >> 6);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 2;
        }
        else if (unicode >= 0x0800 && unicode <= 0xffff)
        {
            *tmp = 0xe0 | (unicode >> 12);
            tmp += 1;
            *tmp = 0x80 | (unicode >> 6 & 0x00ff);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 3;
        }

    }

    *tmp = '\0';
    *out = result;
    return 0;
}

int utf8_to_unicode(unsigned char *in, unsigned short **out, int *outsize)
{
    unsigned char *p = in;
    unsigned short *result = NULL;
    int resultsize = 0;
    unsigned char *tmp = NULL;

    result = (unsigned short *)malloc(strlen((char*)in) * 2 + 2); /* should be enough */
    memset(result, 0, strlen((char*)in) * 2 + 2);
    tmp = (unsigned char *)result;

    while(*p)
    {
        if (*p >= 0x00 && *p <= 0x7f)
        {
            *tmp = *p;
            tmp++;
            *tmp = '\0';
            resultsize += 2;
        }
        else if ((*p & (0xff << 5))== 0xc0)
        {
   //         unsigned short t = 0;
            unsigned char t1 = 0;
            unsigned char t2 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);

            *tmp = t2 | ((t1 & (0xff >> 6)) << 6);//t1 >> 2;
            tmp++;

            *tmp = t1 >> 2;//t2 | ((t1 & (0xff >> 6)) << 6);
            tmp++;

            resultsize += 2;
        }
        else if ((*p & (0xff << 4))== 0xe0)
        {
    //        unsigned short t = 0;
            unsigned char t1 = 0;
            unsigned char t2 = 0;
            unsigned char t3 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);
            p++;
            t3 = *p & (0xff >> 2);

            //Little Endian
            *tmp = ((t2 & (0xff >> 6)) << 6) | t3;//(t1 << 4) | (t2 >> 2);
            tmp++;

            *tmp = (t1 << 4) | (t2 >> 2);//((t2 & (0xff >> 6)) << 6) | t3;
            tmp++;
            resultsize += 2;
        }

        p++;
    }

    *tmp = '\0';
    tmp++;
    *tmp = '\0';
    resultsize += 2;

    *out = result;
    *outsize = resultsize; 
    return 0;
}


/*

int main()
{
    unsigned short unicode[] = L"ÖÐÎÄ";
    unsigned char *utf8 = NULL;

    int unisize = 0;
    unsigned short *uni = NULL;

    printf("original unicode: \n");
    dump_unicode(unicode, sizeof(unicode));
    
    printf("converted to utf8: \n");
    unicode_to_utf8(unicode, sizeof(unicode), &utf8);
    dump_utf8(utf8);

    printf("converted to unicode: \n");
    utf8_to_unicode(utf8, &uni, &unisize);
    dump_unicode(uni, unisize);

    free(utf8);
    free(uni);

    return 0;
}
*/

