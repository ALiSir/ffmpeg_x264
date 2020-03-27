
#include <jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/avutil.h"
#include <stdio.h>
#include <android/log.h>
#define LOG_TAG "sanbot_decode"
#define Lg(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)

#include <sys/time.h>
long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// AVPacket avpkt;
AVCodecContext *context;
AVCodec *codec;
AVFrame *pFrame_ = NULL;
AVFrame *yuv_frame = NULL;
int runState = 0;

void ffInit()
{

    avcodec_register_all();
    // Lg(" --- %d",AV_CODEC_PROP_REORDER);
    // av_init_packet(&avpkt);

    // avcodec_init();

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);

    context = avcodec_alloc_context3(codec);

    Lg(" h264 code = %d", codec);
    if (!codec)
    {
        Lg("no find h264 code");
        return;
    }

    context->time_base.num = 1;
    context->frame_number = 1;
    context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->bit_rate = 0;
    context->time_base.den = 30; //帧率
    // context->width = 1280;       //视频宽
    // context->height = 720;       //视频高

    int ret = avcodec_open2(context, codec, 0);
    Lg("open ret = %d", ret);
    if (avcodec_open2(context, codec, 0) < 0)
    {
        Lg("av_frame_allo fail ");
        return;
    }

    yuv_frame = av_frame_alloc();

    // pFrame_ = avcodec_alloc_frame(); // Allocate video frame

    // AVFormatContext *pFormatCtx = NULL;
    // AVCodecContext *pCodecCtx = NULL;
    // AVCodec *pCodec = NULL;
    // AVFrame *pFrame = NULL,*pFrameYUV = NULL;
    // unsigned char *out_buffer = NULL;
    // AVPacket packet;
    // struct SwsContext *img_convert_ctx = NULL;
    // int got_picture;
    // int videoIndex;
    // int frame_cnt=1;
    runState = 1;
}

void decodeH264(int width, int height, uint8_t *inBuf, int inSize, uint8_t *outBuf, int *outSize)
{
    if(!runState){
        Lg("decode need init ! please initialize first !");
        return;
    }
    long decStTime = getCurrentTime();
    AVPacket avpkt = {0};
    avpkt.data = inBuf;
    avpkt.size = inSize;

    // if (context->width != width)
    // {
    //     context->width = width;
    // }
    // if (context->height != height)
    // {
    //     context->height = height;
    // }

    int ret = avcodec_send_packet(context, &avpkt);
    if (ret == 0)
    {

        if (avcodec_receive_frame(context, yuv_frame) == 0)
        {
            Lg("buf size = %d ; data size = %d ; height = %d ; witdh = %d ; y.size = %d ; u.size = %d ; v.size = %d", sizeof(yuv_frame->buf), sizeof(yuv_frame->data[0]), yuv_frame->height, yuv_frame->width, yuv_frame->linesize[0], yuv_frame->linesize[1], yuv_frame->linesize[2]);
            // yuv_frame->format

            for (int i = 0; i < yuv_frame->height; i++)
            {
                memcpy(outBuf + yuv_frame->linesize[0] * i, yuv_frame->data[0] + yuv_frame->linesize[0] * i, yuv_frame->linesize[0]);
            }

            Lg("decode y component finsh");

            int ysize = yuv_frame->height * yuv_frame->width;
            long stTime = getCurrentTime();
            for (int i = 0; i < yuv_frame->height; i++)
            {
                for (int j = 0; j < yuv_frame->width; j++)
                {
                    if (j % 2 == 0)
                    {
                        //save v
                        memcpy(outBuf + ysize + i * yuv_frame->width + j, yuv_frame->data[2] + yuv_frame->linesize[2] * i + j / 2, 1);
                    }
                    else
                    {
                        //save u
                        memcpy(outBuf + ysize + i * yuv_frame->width + j, yuv_frame->data[1] + yuv_frame->linesize[1] * i + j / 2, 1);
                    }
                }
            }
            Lg("耗时：%d", getCurrentTime() - stTime);
            *outSize = ysize + ysize;

            // int a = 0, i;
            // int height = yuv_frame->height;
            // int width = yuv_frame->width;
            // for (i = 0; i < height; i++)
            // {
            //     memcpy(outBuf + a, yuv_frame->data[0] + i * yuv_frame->linesize[0], width);
            //     a += width;
            // }
            // for (i = 0; i < height / 2; i++)
            // {
            //     memcpy(outBuf + a, yuv_frame->data[2] + i * yuv_frame->linesize[2], width / 2);
            //     a += width / 2;
            // }
            // for (i = 0; i < height / 2; i++)
            // {
            //     memcpy(outBuf + a, yuv_frame->data[1] + i * yuv_frame->linesize[1], width / 2);
            //     a += width / 2;
            // }

            // *outSize = a;

            // for (int i = 0; i < yuv_frame->height; i++)
            //     memcpy(outBuf + yuv_frame->width * i, yuv_frame->data[0] + yuv_frame->linesize[0] * i, yuv_frame->width);

            // int ysize = yuv_frame->height * yuv_frame->width;

            // for (int j = 0; j < yuv_frame->height / 2; j++)
            //     memcpy(outBuf + yuv_frame->width / 2 * j + ysize, yuv_frame->data[1] + yuv_frame->linesize[1] * j, yuv_frame->width / 2);

            // for (int k = 0; k < yuv_frame->height / 2; k++)
            //     memcpy(outBuf + yuv_frame->width / 2 * k + ysize * 5 / 4, yuv_frame->data[2] + yuv_frame->linesize[2] * k, yuv_frame->width / 2);

            // *outSize = ysize + ysize;
        }
        else
        {
            Lg("解码失败：接收pc失败！");
        }
        // Lg("开始释放frame");
        // av_frame_free(yuv_frame);
        // Lg("释放完成");
    }
    else
    {
        Lg("解码失败:发送pc失败！ret = %d", ret);
    }
    // Lg("开始释放avpkt");
    av_packet_free(&avpkt);
    // Lg("avpkt 释放完成");
    // Lg("解码成功");
    Lg("总耗时：%d", getCurrentTime() - decStTime);
    return;
}

void ffdestroy()
{
    runState = 0;
    // av_packet_unref(&avpkt);
    av_frame_free(yuv_frame);
    Lg("close decode");
}
