#include "aes_decode.h"
#include "ffmpeg/openssl/aes.h"
#include "alog.h"
//#include "qh_ptc.h"


#define SEG_LEN 60000



void KeyAddrInit(int* key_addr)
{
    key_addr[0] = 0x603deb10;
    key_addr[1] = 0x15ca71be;
    key_addr[2] = 0x2b73aef0;
    key_addr[3] = 0x857d7781;
    key_addr[4] = 0x1f352c07;
    key_addr[5] = 0x3b6108d7;
    key_addr[6] = 0x2d9810a3;
    key_addr[7] = 0x0914dff4;
}

void IVAddrInit(int* IV_addr)
{
    IV_addr[0] = 0x00010203;
    IV_addr[1] = 0x04050607;
    IV_addr[2] = 0x08090a0b;
    IV_addr[3] = 0x0c0d0e0f;
}

int Aes_Decode(unsigned char* data,int* len,int paddingbytes)
{

    unsigned char buf2[SEG_LEN];
    unsigned char aes_keybuf[32];
    unsigned int key_addr[8];
    unsigned int IV_addr[4];
    char ivec[40];
    AES_KEY aeskey;

    KeyAddrInit(key_addr);
    IVAddrInit(IV_addr);

    memcpy((char*)aes_keybuf,(char*)key_addr,sizeof(key_addr)/2);
    memset(ivec,0,sizeof(ivec));
    memcpy((char*)ivec,(char*)IV_addr,sizeof(IV_addr));


    if(AES_set_decrypt_key(aes_keybuf,128,&aeskey)<0)
        return -1;

    int length = *len;
    int loop = 0;

    loop = length/SEG_LEN;
    loop += (length%SEG_LEN)?1:0;

    int reset = length;
    int interval = 0;
    int i = 0;
    //LOGE("loop = %d length = %d SEG_LEN = %d",loop,length,SEG_LEN);

    for(i=0;i<loop;i++)
    {

        memcpy((char*)aes_keybuf,(char*)key_addr,sizeof(key_addr)/2);
        memset(ivec,0,sizeof(ivec));
        memcpy((char*)ivec,(char*)IV_addr,sizeof(IV_addr));

        if(AES_set_decrypt_key(aes_keybuf,128,&aeskey)<0)
            return -1;

        memset(buf2,0,sizeof(buf2));

        interval = i*SEG_LEN;

        if(reset > SEG_LEN)
        {
            AES_cbc_encrypt((unsigned char*)data+interval,buf2,SEG_LEN,&aeskey,(unsigned char *)ivec,AES_DECRYPT);

            memcpy(data+interval,buf2,SEG_LEN);
            reset -= SEG_LEN;
        }
        else
        {
            AES_cbc_encrypt((unsigned char*)data+interval,buf2,reset,&aeskey,(unsigned char *)ivec,AES_DECRYPT);
            memcpy(&data[interval],buf2,reset-paddingbytes);
            reset = 0;
        }

    }

    *len = length - paddingbytes;
    //LOGE("data %d",data[0]);

    return *len;
}

