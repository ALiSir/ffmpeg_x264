#ifndef __AES_DECODE_H__
#define __AES_DECODE_H__

#ifdef __cplusplus
extern "C"{
#endif

int Aes_Decode(unsigned char* data,int* len,int paddingbytes);
//int Aes_Decode1(char* pFrame);

#ifdef __cplusplus
}
#endif
#endif
