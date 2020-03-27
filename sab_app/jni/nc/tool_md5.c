
#if defined __cplusplus
extern "C"
{
#endif

#include "tool_type.h"
#include "tool_md5.h"
#include "tool_log.h"
#include "tool_sysf.h"

/* Constants for MD5Transform routine.*/
#define S11                     7
#define S12                     12
#define S13                     17
#define S14                     22
#define S21                     5
#define S22                     9
#define S23                     14
#define S24                     20
#define S31                     4
#define S32                     11
#define S33                     16
#define S34                     23
#define S41                     6
#define S42                     10
#define S43                     15
#define S44                     21


/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z)              (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z)              (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z)              ((x) ^ (y) ^ (z))
#define I(x, y, z)              ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n)       (((x) << (n)) | ((x) >> (32-(n))))

/** 
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac)       \
                    do { \
                        (a) += F ((b), (c), (d)) + (x) + (ac); \
                        (a) = ROTATE_LEFT ((a), (s)); \
                        (a) += (b); \
                    } while (0)

#define GG(a, b, c, d, x, s, ac)      \
                    do { \
                        (a) += G ((b), (c), (d)) + (x) + (ac); \
                        (a) = ROTATE_LEFT((a), (s)); \
                        (a) += (b); \
                    } while (0)

#define HH(a, b, c, d, x, s, ac)      \
                    do{ \
                        (a) += H ((b), (c), (d)) + (x) + (ac); \
                        (a) = ROTATE_LEFT ((a), (s)); \
                        (a) += (b); \
                    } while (0)

#define II(a, b, c, d, x, s, ac)       \
                do { \
                    (a) += I ((b), (c), (d)) + (x) + (ac); \
                    (a) = ROTATE_LEFT ((a), (s)); \
                    (a) += (b); \
                } while (0)


TOOL_STATIC TOOL_CONST TOOL_UINT8 PADDING[64] = 
{
    0x80, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0
};

TOOL_STATIC TOOL_VOID tool_md5_encode(TOOL_IN TOOL_UINT32 *pu32Input, TOOL_IN TOOL_UINT32 ulLen, TOOL_OUT TOOL_UINT8 *pcOutput)
{
    TOOL_UINT32 i, j;    

    for (i = 0, j = 0; j < ulLen; i++, j += 4) 
    {
        pcOutput[j]   = (TOOL_UINT8)(pu32Input[i] & 0xff);
        pcOutput[j+1] = (TOOL_UINT8)((pu32Input[i] >> 8) & 0xff);
        pcOutput[j+2] = (TOOL_UINT8)((pu32Input[i] >> 16) & 0xff);
        pcOutput[j+3] = (TOOL_UINT8)((pu32Input[i] >> 24) & 0xff);
    }

    return;
}

TOOL_STATIC TOOL_VOID tool_md5_decode(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulLen, TOOL_OUT TOOL_UINT32 *pu32Output)
{
    TOOL_UINT32 i, j;
    
    for (i = 0, j = 0; j < ulLen; i++, j += 4)
    {
        pu32Output[i] = pucInput[j] | (pucInput[j+1] << 8) |
                        (pucInput[j+2] << 16) | (pucInput[j+3] << 24);
    }

    return;
}

/* MD5 basic transformation. Transforms state based on block. */
TOOL_STATIC TOOL_VOID tool_md5_transform(TOOL_IN TOOL_UINT32 u32State[4], TOOL_IN TOOL_CONST TOOL_UINT8 ucBlock[64])
{
    TOOL_UINT32 a = u32State[0];
    TOOL_UINT32 b = u32State[1];
    TOOL_UINT32 c = u32State[2];
    TOOL_UINT32 d = u32State[3];
    TOOL_UINT32 x[16];

    tool_md5_decode(ucBlock, 64, x);
    
    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
    
    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
    
    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34, 0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */
    
    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
    
    u32State[0] += a;
    u32State[1] += b;
    u32State[2] += c;
    u32State[3] += d;

    return;
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
TOOL_STATIC TOOL_VOID tool_md5_init(TOOL_OUT tool_md5_t *pstContext)
{
    pstContext->u32Cnt[0] = pstContext->u32Cnt[1] = 0;

    /* Load magic initialization constants. */
    pstContext->u32State[0] = 0x67452301;
    pstContext->u32State[1] = 0xefcdab89;
    pstContext->u32State[2] = 0x98badcfe;
    pstContext->u32State[3] = 0x10325476;

    return;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
TOOL_STATIC TOOL_VOID tool_md5_update(TOOL_IN tool_md5_t *pstContext, TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInputLen)
{
    TOOL_UINT32 i;
    TOOL_UINT32 ulIndex;
    TOOL_UINT32 ulPartLen;
    
    /* Compute number of bytes mod 64 */
    ulIndex = ( (pstContext->u32Cnt[0] >> 3) & 0x3F);
    
    /* Update number of bits */
    pstContext->u32Cnt[0] += (ulInputLen << 3);
    if ( pstContext->u32Cnt[0] < (ulInputLen << 3) )
    {
        pstContext->u32Cnt[1] ++;
    }
    pstContext->u32Cnt[1] += (ulInputLen >> 29);
    
    ulPartLen = 64 - ulIndex;
    /* Transform as many times as possible. */
    if (ulInputLen >= ulPartLen)
    {
        tool_mem_memcpy(&pstContext->szBuffer[ulIndex], pucInput, ulPartLen);
        tool_md5_transform(pstContext->u32State, pstContext->szBuffer);
        
        for (i = ulPartLen; i + 63 < ulInputLen; i += 64)
        {
            tool_md5_transform(pstContext->u32State, &pucInput[i]);
        }
        ulIndex = 0;
    }
    else
    {
        i = 0;
    }
    /* Buffer remaining input */
    tool_mem_memcpy(&pstContext->szBuffer[ulIndex], &pucInput[i], ulInputLen - i);

    return;
}

/**
 * MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */
TOOL_STATIC TOOL_VOID tool_md5_final(TOOL_IN tool_md5_t *pstContext, TOOL_OUT TOOL_UINT8 ucDigest[16])
{
    TOOL_UINT8 bits[8];
    TOOL_UINT32 ulIndex, padLen;
    
    /* Save number of bits */
    tool_md5_encode(pstContext->u32Cnt, 8, bits);
    
    /* Pad out to 56 mod 64. */
    ulIndex = ((pstContext->u32Cnt[0] >> 3) & 0x3f);
    padLen  = (ulIndex < 56) ? (56 - ulIndex) : (120 - ulIndex);
    tool_md5_update(pstContext, PADDING, padLen);
    
    /* Append length (before padding) */
    tool_md5_update(pstContext, bits, 8);
    
    /* Store state in digest */
    tool_md5_encode(pstContext->u32State, 16, ucDigest);

    return;
}

#if 0
/* ---------------这仅仅是分割线------------------------ */
#endif

TOOL_VOID tool_md5_calc(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_UINT8 *pucOutput)
{
    tool_md5_t stContext;

    tool_md5_init(&stContext);
    tool_md5_update(&stContext, pucInput, ulInlen);
    tool_md5_final(&stContext, pucOutput);

    return;
}

TOOL_INT32 tool_md5_check(TOOL_IN TOOL_UINT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen)
{
    TOOL_UINT8 authBuf[16];

    tool_md5_calc(input, inlen, authBuf);
    if ( tool_mem_memcmp(authenticator, authBuf, 16) )
    {
        return 0;
    }
    return -1;
}

TOOL_VOID tool_md5_calc_16bit(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_UINT8 *pucOutput)
{
    TOOL_UINT8 tmp_buf[16] = {0};
	tool_md5_calc(pucInput, ulInlen, tmp_buf);
	tool_mem_memcpy(pucOutput, &tmp_buf[4], 8);
}

TOOL_INT32 tool_md5_check_16bit(TOOL_IN TOOL_UINT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen)
{
    TOOL_UINT8 authBuf[16];

    tool_md5_calc(input, inlen, authBuf);
    if ( tool_mem_memcmp(authenticator, &authBuf[4], 8) )
    {
        return 0;
    }
    return -1;
}

TOOL_VOID tool_md5_calc_xm(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_INT8 *pucOutput)
{
    TOOL_UINT8 tmp_buf[16] = {0};
	tool_md5_calc(pucInput, ulInlen, tmp_buf);

	TOOL_INT32 i = 0;
	for (i = 0; i < 8; i++)
	{
		pucOutput[i] = ( (tmp_buf[2 * i] + tmp_buf[2 * i + 1]) % 62 );

		if ((pucOutput[i] >= 0) && (pucOutput[i] <= 9))
		{
			pucOutput[i] += 48;
		}
		else
		{

			if ((pucOutput[i] >= 10) && (pucOutput[i] <= 35))
			{
				pucOutput[i] += 55;
			}
			else
			{
				pucOutput[i] += 61;
			}
		}		
//		log_debug("i(%d) ch(%d)(%c)", i, pucOutput[i], pucOutput[i]);
	}
}

TOOL_INT32 tool_md5_check_xm(TOOL_IN TOOL_INT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen)
{
    TOOL_INT8 authBuf[16];

    tool_md5_calc_xm(input, inlen, authBuf);
    if ( tool_mem_memcmp(authenticator, authBuf, 8) )
    {
        return 0;
    }
    return -1;
}



#if defined __cplusplus
}
#endif

