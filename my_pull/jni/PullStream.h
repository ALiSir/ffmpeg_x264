#include <jni.h>

typedef struct avcodec_jni
{

    int handle;
    int audio_tick;
    int audio_len;

    char *audiobuf; //audio buf

    long vd_handle; //decode struct addr
    char *tempbuf;  //decoded video buf addr

    int frame_sub_type;
    int width;
    int height;

    int hardDecode;
    int justIframe;

} av_t;


typedef struct
{
    unsigned int len;
    unsigned int no;
    unsigned int sec;
    unsigned int usec;
    unsigned short width;
    unsigned short height;
    unsigned char frame_type;     // 1=I, 2=P, 3=A, 255=video end frame PTC_FRAME_E
    unsigned char frame_sub_type; //frame_type=1/2: PTC_VIDEO_ENCODE_E PTC_AUDIO_ENCODE_E
    unsigned char frame_rate;
    unsigned char security;
    unsigned char padding_bytes;
    unsigned char channel_type; //channel type PTC_CH_TYPE_E
    unsigned char channel_no;   //channel num
    unsigned char reserve[1];
    unsigned int flag; //PTC_FRAME_FLAG
} ptc_frame_head_t;

#define AUDIO_IN_SECOND 2 * 8192 + 100
#define AUDIO_BUF_LEN 1024 * 8        //audio frame cache buf size
#define VIDEO_BUF_LEN 4 * 1024 * 1024 //video frame cache buf size


void StreamInit();
int openStream(int ch, int hardDecode, int justIframe,*streamCallBack(int handle, unsigned char *buf, int len, void *pUser));