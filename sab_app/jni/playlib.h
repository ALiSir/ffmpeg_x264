#ifndef __PLAYLIB_H__
#define __PLAYLIB_H__

#include "ffmpeg/libavcodec/avcodec.h"


#ifdef __cplusplus
 extern "C"{
#endif

typedef enum eVideoFormat{
  VIDEO_FORMAT_UNKOWN = 0,
  VIDEO_FORMAT_HARD = 1,
  VIDEO_FORMAT_H264_RGB565 = 2,
  VIDEO_FORMAT_H264_NV21 = 3,
  VIDEO_FORMAT_MJPEG = 4,
}VideoFormat;


typedef struct structVideoDecode {
    AVCodec*        mAVCodec;
    AVCodecContext* mAVCodecContext;
    AVFrame*        mAVFrame;
    AVPicture       mAVPicture;
   struct SwsContext*     mSwsContext;
   int pix_fmt;
   int codeid;
} videodecode_t;



void ffmpeg_init();
void ffmpeg_done();

int video_began(int video_format, int image_width, int image_height, long *vd_handle);
void video_ended(long* vd_handle);
int video_decode(long vd_handle, uint8_t* in_264Buffer, int in_264BufferSize, uint8_t* out_RGB24Buffer, int* out_image_width, int* out_image_height,int* outsize);

#ifdef __cplusplus
 }
#endif
#endif
