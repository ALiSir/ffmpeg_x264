#include "playlib.h"
#include "ffmpeg/libavutil/pixfmt.h"
#include "ffmpeg/libswscale/swscale.h"
//#include "alog.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR , "FFMPEG", __VA_ARGS__)

void ffmpeg_init()
{
    //av_register_all();
    avcodec_register_all();
}
void ffmpeg_done()
{
    //avcodec_register_all();
}

int video_began(int video_format, int image_width, int image_height, long* vd_handle)
{
    videodecode_t* vd = (videodecode_t*)malloc(sizeof(videodecode_t));

    if(vd == 0)
        return -1;
    memset(vd,0,sizeof(sizeof(videodecode_t)));
    *vd_handle = (long)vd;

    enum AVCodecID codeId = AV_CODEC_ID_NONE;
    int pix_fmt = AV_PIX_FMT_RGB565LE;

    switch(video_format)
    {
        case VIDEO_FORMAT_H264_RGB565:
            codeId = AV_CODEC_ID_H264;
            pix_fmt = AV_PIX_FMT_RGB565LE;
            break;
        case VIDEO_FORMAT_H264_NV21:
            codeId = AV_CODEC_ID_H264;
            pix_fmt = PIX_FMT_NV21;
            break;
        case VIDEO_FORMAT_MJPEG:
            codeId = AV_CODEC_ID_MJPEG;
            break;
        default:
            return -2;
    }
    vd->pix_fmt = pix_fmt;
    vd->codeid = codeId;
    vd->mAVCodec = avcodec_find_decoder(codeId);
    if(!vd->mAVCodec)
    {
        return -3;
    }

    vd->mAVCodecContext = avcodec_alloc_context3(vd->mAVCodec);
    if(!vd->mAVCodecContext)
    {
        return -4;
    }

    vd->mAVCodecContext->width = image_width;
    vd->mAVCodecContext->height = image_height;
    vd->mAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(vd->mAVCodecContext, vd->mAVCodec, 0) < 0) // 闇�瑕佸姞閿�
    {
        return -5;
    }

    vd->mAVFrame = av_frame_alloc();
    if (!vd->mAVFrame)
    {
        return -6;
    }
    if(avpicture_alloc(&vd->mAVPicture, pix_fmt, vd->mAVCodecContext->width, vd->mAVCodecContext->height)<0)
    {
        return -7;
    }

    vd->mSwsContext = sws_getCachedContext(0,
                                           vd->mAVCodecContext->width,
                                           vd->mAVCodecContext->height,
                                           vd->mAVCodecContext->pix_fmt,
                                           vd->mAVCodecContext->width,          // 鐢熸垚鍥惧儚鐨勫搴�
                                           vd->mAVCodecContext->height,         // 鐢熸垚鍥惧儚鐨勯珮搴�
										   pix_fmt,
                                           SWS_FAST_BILINEAR, NULL, NULL, NULL);//SWS_POINT?

    return 0;
}

void video_ended(long* vd_handle)
{
    if (*vd_handle == 0)
        return ;

    videodecode_t* vd = (videodecode_t*)*vd_handle;
    if(vd->mAVCodecContext)
    {
     //   LOGD("video_ended: %d ", __LINE__);
        avcodec_close(vd->mAVCodecContext); // 闇�瑕佸姞閿�
        av_free(vd->mAVCodecContext);
        vd->mAVCodecContext = 0;
    }

    if(vd->mAVFrame)
    {
      //  LOGD("video_ended: %d ", __LINE__);
        av_free(vd->mAVFrame);
        vd->mAVFrame = 0;
    }
    avpicture_free(&vd->mAVPicture);
    

    if (vd->mSwsContext)
    {
     //   LOGD("video_ended: %d ", __LINE__);
        sws_freeContext(vd->mSwsContext);
    }

  //  LOGD("video_ended: %d ", __LINE__);
    free(vd);
    vd = 0;
    *vd_handle = 0;
 //   LOGD("video_ended: %d ", __LINE__);
}
//#include "alog.h"
//#include <sys/time.h>
int video_decode(long vd_handle, uint8_t* in_264Buffer, int in_264BufferSize, uint8_t* out_RGB24Buffer, int* out_image_width, int* out_image_height,int* outsize)
{

    if (vd_handle == 0 || in_264Buffer == 0 || in_264BufferSize == 0)
        return -1;

  //  LOGD("video_ended111: %d ", __LINE__);

    videodecode_t* vd = (videodecode_t*)vd_handle;
    if (vd == NULL) return -1;

    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = in_264Buffer;    // packet data will be allocated by the encoder
    avpkt.size = in_264BufferSize;
    //struct timeval startTime;
    //struct timeval endTime;
    //struct timeval centerTime;
    //memset(&startTime,0,sizeof(struct timeval));
    //memset(&endTime,0,sizeof(struct timeval));
    //memset(&centerTime,0,sizeof(struct timeval));
  //  LOGD("video_ended222: %d ", __LINE__);
    //gettimeofday(&startTime,NULL);
    int got_picture = 0;
    if (vd->mAVCodecContext == NULL) return -1;

    int consumed_bytes = avcodec_decode_video2(vd->mAVCodecContext, vd->mAVFrame, &got_picture, &avpkt);
  //  LOGD("video_ended3333: %d ", __LINE__);
    av_free_packet(&avpkt);
    if (consumed_bytes <= 0 || vd->mAVFrame->data[0] == 0)
    {
        return -2;
    }
  //  LOGD("video_ended4444: %d", __LINE__);
    //gettimeofday(&centerTime,NULL);
    // 鎶婃暟鎹粠pFrame鍐欏埌picture涔嬩腑
   // LOGD("vd->mAVPicture.data[1] %d ",vd->mAVFrame->data[0]);
    sws_scale(vd->mSwsContext,
              (const uint8_t *const *)vd->mAVFrame->data,
              vd->mAVFrame->linesize,
              0,
              vd->mAVCodecContext->height,
              vd->mAVPicture.data,
              vd->mAVPicture.linesize);

    int size = vd->mAVPicture.linesize[0] * vd->mAVCodecContext->height;

    if(vd->pix_fmt == PIX_FMT_NV21){
		memcpy(out_RGB24Buffer, vd->mAVPicture.data[0],size);
		memcpy(out_RGB24Buffer+size, vd->mAVPicture.data[1],size/2);
        // LOGD("vd->mAVPicture.data[0] %d  %d",vd->mAVPicture.data[0],vd->mAVPicture.data[1]);
		*out_image_width = vd->mAVCodecContext->width;
		*out_image_height = vd->mAVCodecContext->height;
		*outsize = size+size/2;
    }else if(vd->pix_fmt == AV_PIX_FMT_RGB565LE){

	   //  LOGD("video_ended4444222: %d", size);
		// LOGD("vd->mAVPicture.linesize = %d ",vd->mAVPicture.linesize[0]);
	   //  LOGD("vd->mAVPicture.data[1] %d ",vd->mAVPicture.data[0]);

		// LOGD("out_RGB24Buffer len = %d", sizeof(out_RGB24Buffer));
		 memcpy(out_RGB24Buffer, vd->mAVPicture.data[0], size);
		// LOGD("video_ended555: %d ", __LINE__);
		 *out_image_width = vd->mAVCodecContext->width;
		 *out_image_height = vd->mAVCodecContext->height;
		 *outsize = size;
    }

    return size;
}

