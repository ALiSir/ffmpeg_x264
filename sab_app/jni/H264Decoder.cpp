/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <jni.h>  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include "yuv2rgb.h"
#include "yuv2rgb16tab.c"
#include "playlib.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <list>

#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR , "FFMPEG", __VA_ARGS__)
#include<pthread.h>

#ifdef __cplusplus
    extern "C" {
#endif

static pthread_mutex_t lock;

#include <ffmpeg/libavcodec/avcodec.h>

#define RENDER_MAX_Y_SIZE 	(2600 * 1600 *3/2)
#define RENDER_MAX_UV_SIZE 	(RENDER_MAX_Y_SIZE/4)

typedef unsigned char byte_t; 
typedef unsigned char BYTE; 

/**
 * @struct tagNETDEVPictureData
 * @brief 图像数据 结构体定义 Image data Structure definition
 *        存放解码后图像数据的指针和长度等信息 Storing pointer and length of decoded picture data
 * @attention 无 None
 */
typedef struct tagNETDEVPictureData
{
    BYTE    *pucData[4];                /* pucData[0]:Y 平面指针,pucData[1]:U 平面指针,pucData[2]:V 平面指针  pucData[0]: Y plane pointer, pucData[1]: U plane pointer, pucData[2]: V plane pointer */
    uint32_t   dwLineSize[4];              /* ulLineSize[0]:Y平面每行跨距, ulLineSize[1]:U平面每行跨距, ulLineSize[2]:V平面每行跨距  ulLineSize[0]: Y line spacing, ulLineSize[1]: U line spacing, ulLineSize[2]: V line spacing */
    uint32_t   dwPicHeight;                /* 图片高度  Picture height */
    uint32_t   dwPicWidth;                 /* 图片宽度  Picture width */
    uint32_t   dwRenderTimeType;           /* 用于渲染的时间数据类型  Time data type for rendering */
    uint64_t   tRenderTime;                /* 用于渲染的时间数据  Time data for rendering */
}NETDEV_PICTURE_DATA_S, *LPNETDEV_PICTURE_DATA_S;

static inline uint8_t CLAMP(int16_t x)
{
    return  ((x > 255) ? 255 : (x < 0) ? 0 : x);
}

typedef struct 
{
struct AVCodec *fCodec;			  // Codec
struct AVCodecContext *fCodecContext;		  // Codec Context
struct AVFrame *fVideoFrame;		  // Frame	
int fDisplayWidth;
int fDisplayHeight;
int *fColorTable;
ANativeWindow *mRenderWin;
NETDEV_PICTURE_DATA_S mPictureData;
char* mBmpBuf;
int mBmpSize;
unsigned int *rgb_2_pix;
unsigned int *r_2_pix;
unsigned int *g_2_pix;
unsigned int *b_2_pix;
}Decoder; 

int avcodec_decode_video1(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const uint8_t *buf, int buf_size)
{
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = buf;
    avpkt.size = buf_size;
    // HACK for CorePNG to decode as normal PNG by default
    avpkt.flags = AV_PKT_FLAG_KEY;
    return avcodec_decode_video2(avctx, picture, got_picture_ptr, &avpkt);
}

#define RGB_V(v) ((v < 0) ? 0 : ((v > 255) ? 255 : v))
		
void DeleteYUVTable(Decoder* decoder)
{
	av_free(decoder->fColorTable); 
    av_free(decoder->rgb_2_pix);
}

void CreateYUVTable(Decoder *decoder)
{
	int i;
	int u, v;
	int *u_b_tab = NULL;
	int *u_g_tab = NULL;
	int *v_g_tab = NULL;
	int *v_r_tab = NULL;
 
	decoder->fColorTable = (int *)av_malloc(4 * 256 * sizeof(int));
	u_b_tab = &decoder->fColorTable[0 * 256];
	u_g_tab = &decoder->fColorTable[1 * 256];
	v_g_tab = &decoder->fColorTable[2 * 256];
	v_r_tab = &decoder->fColorTable[3 * 256];

	for (i = 0; i < 256; i++) {
		u = v = (i - 128);
		u_b_tab[i] = (int) ( 1.772   * u);
		u_g_tab[i] = (int) ( 0.34414 * u);
		v_g_tab[i] = (int) ( 0.71414 * v); 
		v_r_tab[i] = (int) ( 1.402   * v);
	}
}

void CreateYUVTab_16(Decoder *decoder)
{
    int i;
    int u, v;
    int *u_b_tab=NULL;
	int *u_g_tab=NULL;
	int *v_g_tab=NULL;
	int *v_r_tab=NULL; 

    decoder->fColorTable = (int *)av_malloc(4*256*sizeof(int));
    u_b_tab = &decoder->fColorTable[0*256];
    u_g_tab = &decoder->fColorTable[1*256];
    v_g_tab = &decoder->fColorTable[2*256];
    v_r_tab = &decoder->fColorTable[3*256];

    for (i=0; i<256; i++)
    {
        u = v = (i-128);

        u_b_tab[i] = (int) ( 1.772 * u);
        u_g_tab[i] = (int) ( 0.34414 * u);
        v_g_tab[i] = (int) ( 0.71414 * v); 
        v_r_tab[i] = (int) ( 1.402 * v);
    }

    decoder->rgb_2_pix = (unsigned int *)av_malloc(3*768*sizeof(unsigned int));

    decoder->r_2_pix = &decoder->rgb_2_pix[0*768];
    decoder->g_2_pix = &decoder->rgb_2_pix[1*768];
    decoder->b_2_pix = &decoder->rgb_2_pix[2*768];

    for(i=0; i<256; i++)
    {
        decoder->r_2_pix[i] = 0;
        decoder->g_2_pix[i] = 0;
        decoder->b_2_pix[i] = 0;
    }

    for(i=0; i<256; i++)
    {
        decoder->r_2_pix[i+256] = (i & 0xF8) << 8;
        decoder->g_2_pix[i+256] = (i & 0xFC) << 3;
        decoder->b_2_pix[i+256] = (i ) >> 3;
    }

    for(i=0; i<256; i++)
    {
        decoder->r_2_pix[i+512] = 0xF8 << 8;
        decoder->g_2_pix[i+512] = 0xFC << 3;
        decoder->b_2_pix[i+512] = 0x1F;
    }

    decoder->r_2_pix += 256;
    decoder->g_2_pix += 256;
    decoder->b_2_pix += 256;
}

/** YV12 To RGB565 */
void DisplayYUV_16(Decoder *decoder, unsigned int *pdst1, unsigned char *y, unsigned char *u, unsigned char *v, int width, int height, int src_ystride, int src_uvstride, int dst_ystride)
{
	int *u_b_tab = &decoder->fColorTable[0 * 256];
	int *u_g_tab = &decoder->fColorTable[1 * 256];
	int *v_g_tab = &decoder->fColorTable[2 * 256];
	int *v_r_tab = &decoder->fColorTable[3 * 256];
    int i, j;
    int r, g, b, rgb;

    int yy, ub, ug, vg, vr;

    unsigned char* yoff;
    unsigned char* uoff;
    unsigned char* voff;
    
    unsigned int* pdst=pdst1;

    int width2 = width/2;
    int height2 = height/2;
    
    if(width2>decoder->fDisplayWidth/2)
    {
        width2=decoder->fDisplayWidth/2;

        y+=(width-decoder->fDisplayWidth)/4*2;
        u+=(width-decoder->fDisplayWidth)/4;
        v+=(width-decoder->fDisplayWidth)/4;
    }

    if(height2>decoder->fDisplayHeight)
        height2=decoder->fDisplayHeight;

    for(j=0; j<height2; j++) // 一次2x2共四个像素
    {
        yoff = y + j * 2 * src_ystride;
        uoff = u + j * src_uvstride;
        voff = v + j * src_uvstride;

        for(i=0; i<width2; i++)
        {
            yy  = *(yoff+(i<<1));
            ub = u_b_tab[*(uoff+i)];
            ug = u_g_tab[*(uoff+i)];
            vg = v_g_tab[*(voff+i)];
            vr = v_r_tab[*(voff+i)];

            b = yy + ub;
            g = yy - ug - vg;
            r = yy + vr;

            rgb = decoder->r_2_pix[r] + decoder->g_2_pix[g] + decoder->b_2_pix[b];

            yy = *(yoff+(i<<1)+1);
            b = yy + ub;
            g = yy - ug - vg;
            r = yy + vr;

            pdst[(j*dst_ystride+i)] = (rgb)+((decoder->r_2_pix[r] + decoder->g_2_pix[g] + decoder->b_2_pix[b])<<16);

            yy = *(yoff+(i<<1)+src_ystride);
            b = yy + ub;
            g = yy - ug - vg;
            r = yy + vr;

            rgb = decoder->r_2_pix[r] + decoder->g_2_pix[g] + decoder->b_2_pix[b];

            yy = *(yoff+(i<<1)+src_ystride+1);
            b = yy + ub;
            g = yy - ug - vg;
            r = yy + vr;

            pdst [((2*j+1)*dst_ystride+i*2)>>1] = (rgb)+((decoder->r_2_pix[r] + decoder->g_2_pix[g] + decoder->b_2_pix[b])<<16);
        }
    }
}

/** YV12 To RGB888 */
void DisplayYUV_32(Decoder* decoder, uint_t *displayBuffer, int videoWidth, int videoHeight, int outPitch)
{
	int *u_b_tab = &decoder->fColorTable[0 * 256];
	int *u_g_tab = &decoder->fColorTable[1 * 256];
	int *v_g_tab = &decoder->fColorTable[2 * 256];
	int *v_r_tab = &decoder->fColorTable[3 * 256];
	
	// YV12: [Y:MxN] [U:M/2xN/2] [V:M/2xN/2]
	byte_t* y = decoder->fVideoFrame->data[0];
	byte_t* u = decoder->fVideoFrame->data[1];
	byte_t* v = decoder->fVideoFrame->data[2];
	
	int src_ystride  = decoder->fVideoFrame->linesize[0];
	int src_uvstride = decoder->fVideoFrame->linesize[1];
	
	int i, line;
	int r, g, b;
	
	int ub, ug, vg, vr;

	int width  = videoWidth;
	int height = videoHeight;
	
	// 剪切边框
	if (width > decoder->fDisplayWidth) {
		width = decoder->fDisplayWidth;		
		y += (videoWidth - decoder->fDisplayWidth) / 2;
		u += (videoWidth - decoder->fDisplayWidth) / 4;
		v += (videoWidth - decoder->fDisplayWidth) / 4;
	}
	
	if (height > decoder->fDisplayHeight) {
		height = decoder->fDisplayHeight;
	}
 
	for (line = 0; line < height; line++) {
		byte_t* yoff = y + line * src_ystride;
		byte_t* uoff = u + (line / 2) * src_uvstride;
		byte_t* voff = v + (line / 2) * src_uvstride;
		//uint_t* buffer = displayBuffer + (height - line - 1) * outPitch;
		uint_t* buffer = displayBuffer + line * outPitch;
		
		for (i = 0; i < width; i++) {
			ub = u_b_tab[*uoff];
			ug = u_g_tab[*uoff];
			vg = v_g_tab[*voff];
			vr = v_r_tab[*voff];
			
			b = RGB_V(*yoff + ub);
			g = RGB_V(*yoff - ug - vg);
			r = RGB_V(*yoff + vr);
			
			*buffer = 0xff000000 | b << 16 | g << 8 | r;
			
			buffer++;
			yoff ++;
			
			if ((i % 2) == 1) {
				uoff++;
				voff++;
			}
		}
	}
}

int avc_decode(Decoder* decoder, char* buf, int nalLen, char* out)
{
	byte_t* data = (byte_t*)buf;
	int frameSize = 0;
	//len = avcodec_decode_video2(pAVCodecCtx, pAVFrame, &got_picture,
	//			&mAVPacket);
	int ret = avcodec_decode_video1(decoder->fCodecContext, decoder->fVideoFrame, &frameSize, data, nalLen);
	if (ret <= 0) {
		return ret;
	}
	clock_t t1=clock();//从这开始计时
	int width  = decoder->fCodecContext->width;
	int height = decoder->fCodecContext->height;
	//DisplayYUV_32(decoder, (uint32_t*)out, width, height, decoder->fDisplayWidth);
	//DisplayYUV_16(decoder, (int*)out, decoder->fVideoFrame->data[0], decoder->fVideoFrame->data[1], decoder->fVideoFrame->data[2], width, height,
	//	decoder->fVideoFrame->linesize[0], decoder->fVideoFrame->linesize[1], decoder->fDisplayWidth);
	byte_t* y = decoder->fVideoFrame->data[0];
	byte_t* u = decoder->fVideoFrame->data[1];
	byte_t* v = decoder->fVideoFrame->data[2];

	yuv420_2_rgb565(out, y, u, v, width, height, decoder->fVideoFrame->linesize[0], decoder->fVideoFrame->linesize[1], width << 1, yuv2rgb565_table,3);

	clock_t t2=clock();//从这开始计时

	//LOGD("########## yuv to rgb need time = %d", (t2 - t1)/1000);
	/*yuv420_2_rgb565(out,
	                     y,
	                     u,
	                     v,
	                     width,
	                     height,
	                     width,
	                     width>>1,
	                     width<<2,
	                     yuv2bgr565_table,
	                     0);*/
	return ret;	
}


int avc_decode_get_stream(Decoder * decoder, char* out)
{
	if (out == NULL || decoder->fCodecContext == NULL) {
		return -1;
	}

	int width  = decoder->fCodecContext->width;
	int height = decoder->fCodecContext->height;
	CopyYUV_32(decoder, (uint32_t*)out, width, height, decoder->fDisplayWidth);
	return 0;
}

void CopyYUV_32(Decoder * decoder, uint_t *displayBuffer, int videoWidth, int videoHeight, int outPitch)
{	
	// YV12: [Y:MxN] [U:M/2xN/2] [V:M/2xN/2]
	byte_t* y_data = decoder->fVideoFrame->data[0];
	byte_t* u_data = decoder->fVideoFrame->data[1];
	byte_t* v_data = decoder->fVideoFrame->data[2];
	
	int src_ystride  = decoder->fVideoFrame->linesize[0];
	int src_uvstride = decoder->fVideoFrame->linesize[1];

	byte_t* p = (byte_t*)displayBuffer;
	int displayWidth = videoWidth;

	// Y 
	byte_t* yp = y_data;
	int i=0;
	for (i = 0; i < videoHeight; i++) {
		memcpy(p, yp, src_ystride);
		p  += displayWidth;
		yp += src_ystride;
	}

	displayWidth = displayWidth / 2;

	// U 
	byte_t* up = u_data;
	for (i = 0; i < videoHeight / 2; i++) {
		memcpy(p, up, src_uvstride);
		p  += displayWidth;
		up += src_uvstride;
	}

	// V 
	byte_t* vp = v_data;
	for (i = 0; i < videoHeight / 2; i++) {
		memcpy(p, vp, src_uvstride);
		p  += displayWidth;
		vp += src_uvstride;
	}
}

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/HelloJni/HelloJni.java
 */

jlong Java_com_qhcloud_video_H264Decoder_Initlib(JNIEnv* env, jobject thiz)
{
	pthread_mutex_init(&lock, NULL);
	ffmpeg_init();
}

jlong Java_com_qhcloud_video_H264Decoder_InitH264Decoder(JNIEnv* env, jobject thiz, jint width, jint height)
{
	pthread_mutex_lock(&lock);

	long handle = 0;
	int ret = video_began(VIDEO_FORMAT_H264, width, height, &handle);

	if (ret == 0) {
		LOGD("########## InitH264Decoder = %p", handle);
		pthread_mutex_unlock(&lock);
		return handle;
	}
	pthread_mutex_unlock(&lock);
	return -1;
}

jint Java_com_qhcloud_video_H264Decoder_DecoderToPIC(JNIEnv* env, jobject thiz,jlong pDecoder, jbyteArray in, jint nalLen, jbyteArray out)
{
	pthread_mutex_lock(&lock);

	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * Pixel= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	int ret = -1;

	if (pDecoder > 0) {
		int width;
		int height;
		int size;
		ret = video_decode(pDecoder, Buf, nalLen, Pixel, &width, &height, &size);
		//LOGD("########## DecoderToPIC = width:%d height:%d size:%d", width, height, size);
	}

	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);
	pthread_mutex_unlock(&lock);

	return ret;
}

jint Java_com_qhcloud_video_H264Decoder_ReleaseDecoder(JNIEnv* env, jobject thiz,jlong pDecoder)
{
	pthread_mutex_lock(&lock);
	long handle = pDecoder;

	if (handle != 0) {
		LOGD("########## ReleaseDecoder = %p", handle);
		video_ended(&handle);
	}
	pthread_mutex_unlock(&lock);
	return 0;
}

jlong Java_com_qhcloud_video_H264Decoder_InitDecoder(JNIEnv* env, jobject thiz, jint width, jint height)
{
	pthread_mutex_lock(&lock);
	LOGD("Java_com_qhcloud_video_H264Decoder_InitDecoder");
	Decoder * decoder  = (Decoder *)av_malloc(sizeof(Decoder));

	//LOGD(str);
	//avcodec_init();

	avcodec_register_all();
	//av_register_all();

	decoder->fCodec = avcodec_find_decoder(CODEC_ID_H264);

	decoder->fDisplayWidth  = width;
	decoder->fDisplayHeight = height;

	//CreateYUVTable(decoder);	

	decoder->fCodecContext = avcodec_alloc_context3(decoder->fCodec);
	int ret;
    if((ret = avcodec_open2(decoder->fCodecContext, decoder->fCodec, NULL))<0)
	{
    	LOGD("avcodec_open2 failed");
		pthread_mutex_unlock(&lock);
		return ret; // Could not open codec      ******************程序执行到这里返回-22************
	}

	decoder->fVideoFrame  = avcodec_alloc_frame();
	CreateYUVTab_16(decoder);
	if (decoder->fCodecContext == NULL) LOGD("decoder->fCodecContext == NULL");

	if (decoder->fCodec == NULL) LOGD("decoder->fCodec == NULL");
	
	decoder->mBmpSize = 0;
	decoder->mBmpBuf = NULL;
				
	memset(&decoder->mPictureData, 0x00, sizeof(decoder->mPictureData));

    decoder->mPictureData.pucData[0] = (BYTE *) malloc(RENDER_MAX_Y_SIZE);
    decoder->mPictureData.pucData[1] = (BYTE *) malloc(RENDER_MAX_UV_SIZE);
    decoder->mPictureData.pucData[2] = (BYTE *) malloc(RENDER_MAX_UV_SIZE);
	
	LOGD("init decoder handle:%p", decoder); 
	pthread_mutex_unlock(&lock);
	return (jlong)decoder;
}


jint Java_com_qhcloud_video_H264Decoder_UninitDecoder(JNIEnv* env, jobject thiz,jlong pDecoder)
{
	LOGD("UninitDecoder pDecoder:%p", pDecoder); 
	pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 

	if (decoder->fCodecContext) {
		avcodec_close(decoder->fCodecContext);
	    free(decoder->fCodecContext->priv_data);
		free(decoder->fCodecContext);
		decoder->fCodecContext = NULL;
	}

	if (decoder->fVideoFrame) {
		free(decoder->fVideoFrame);
		decoder->fVideoFrame = NULL;
	}

	DeleteYUVTable(decoder); 
	pthread_mutex_unlock(&lock);
	LOGD("release decoder  handle:%p", decoder); 
	return 1;	
}


/**
* 设置视频图像显示比例  Modify image display ratio
*/
JNIEXPORT jint JNICALL Java_com_qhcloud_video_H264Decoder_setRenderView(JNIEnv *env, jobject thiz,jlong pDecoder, jobject javaSurface)
{
	pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 
	jint flag;
	if (decoder) {
		decoder->mRenderWin = ANativeWindow_fromSurface(env, javaSurface);
		LOGD("setRenderView:%p javaSurface:%p", decoder->mRenderWin, javaSurface);
	} else {
		LOGD("setRenderView: decoder->%p", decoder);
	}
	
    pthread_mutex_unlock(&lock);
	
    return 1;
}

/*
 * YUV数据转换为RGB格式并显示到窗口
 * Attention:
 * This is just an example showing how to get yuv data, convert to rgb, and display.
 * In the actual product, we suggest to use OpenGL to do the rendering.
 * */
void Render_RenderYUV(Decoder * decoder, const NETDEV_PICTURE_DATA_S *pstYuv)
{
    int bmpSize = 0;
    int mCropWidth = pstYuv->dwPicWidth;  
    int mCropHeight = pstYuv->dwPicHeight;  

    int bufWidth = (mCropWidth + 1) & ~1;//按2对齐  
    int bufHeight = (mCropHeight + 1) & ~1;

    //LOGD("Show windows %p", decoder->mRenderWin);
    if (decoder->mRenderWin == 0) {
        return;
    }

    // prepare buffer
    bmpSize = pstYuv->dwPicWidth * pstYuv->dwPicHeight * 4;
	//LOGD("Show windows bmpSize:%d", bmpSize);
    if ((decoder->mBmpSize < bmpSize) && (decoder->mBmpBuf != NULL)) {
        free(decoder->mBmpBuf);
        decoder->mBmpBuf = NULL;
    }

    if (decoder->mBmpBuf == NULL) {
        decoder->mBmpBuf = (char *)malloc(bmpSize);
        memset(decoder->mBmpBuf, 0, bmpSize);
        decoder->mBmpSize = bmpSize;
        ANativeWindow_setBuffersGeometry(decoder->mRenderWin, pstYuv->dwPicWidth, pstYuv->dwPicHeight, 0);
    }
	//LOGD("Show windows decoder->mBmpSize:%d", decoder->mBmpSize);
    // convert yuv to rgb
    unsigned char *y = pstYuv->pucData[0];
    unsigned char *u = pstYuv->pucData[1];
    unsigned char *v = pstYuv->pucData[2];

    int cnt = 0;
	int i=0;
	int j=0;
    for (i = 0; i<pstYuv->dwPicHeight; i++)
    {
        for (j=0; j<pstYuv->dwPicWidth; j++)
        {
            int Y = y[i * pstYuv->dwPicWidth + j];
            int U = u[(int)((i/2)*(pstYuv->dwPicWidth/2) + j/2) ];
            int V = v[(int)((i/2)*(pstYuv->dwPicWidth/2) + j/2)];

            decoder->mBmpBuf[cnt++] = CLAMP(Y + 1.402 * (V - 128));
            decoder->mBmpBuf[cnt++] = CLAMP(Y - 0.344 * (U - 128) - 0.714 * (V - 128));
            decoder->mBmpBuf[cnt++] = CLAMP(Y + 1.772 * (U - 128));
            decoder->mBmpBuf[cnt++] = 0; // useless
        }
    }

    // display
    ANativeWindow_Buffer buffer;
	//LOGD("Show windows decoder->mBmpSize:%d line:%d", decoder->mBmpSize, __LINE__);
    if (ANativeWindow_lock(decoder->mRenderWin, &buffer, NULL) == 0) {
        // update buffer
		//LOGD("Show windows pstYuv->dwPicHeight:%d cnt:%d line:%d", pstYuv->dwPicHeight, cnt, __LINE__);
		int h = 0;
        for (h = 0; h < pstYuv->dwPicHeight * 4; ++h) {
			//LOGD("Show windows pstYuv->dwPicHeight:%d cnt:%d h:%d count:%d buffer.stride:%d", pstYuv->dwPicHeight, cnt, h, h * pstYuv->dwPicWidth, buffer.stride);
            memcpy(buffer.bits + (h * buffer.stride), decoder->mBmpBuf + h * pstYuv->dwPicWidth, buffer.stride);
        }

		//LOGD("Show windows decoder->mBmpSize:%d line:%d", decoder->mBmpSize, __LINE__);
        ANativeWindow_unlockAndPost(decoder->mRenderWin);
    }

	//LOGD("Show windows decoder->mBmpSize:%d line:%d", decoder->mBmpSize, __LINE__);
}

/*YUV图像数据解析*/
void Render_UpdateBuf(Decoder * decoder) {
    if (NULL == decoder) {
        return;
    }

    decoder->mPictureData.dwPicHeight = decoder->fCodecContext->height;
    decoder->mPictureData.dwPicWidth = decoder->fCodecContext->width;
    if (NULL == decoder->fVideoFrame->data[0] || NULL == decoder->fVideoFrame->data[1] || NULL == decoder->fVideoFrame->data[2]) {
        return;
    }

    unsigned char * pucDataY = (unsigned char *) decoder->mPictureData.pucData[0];
    unsigned char * pucDataU = (unsigned char *) decoder->mPictureData.pucData[1];
    unsigned char * pucDataV = (unsigned char *) decoder->mPictureData.pucData[2];
    int size = decoder->mPictureData.dwPicHeight * decoder->mPictureData.dwPicWidth;
	int i=0;
	
    for (i = 0; i < 4; i++) {
        decoder->mPictureData.dwLineSize[i] = decoder->fVideoFrame->linesize[i];
    }

    i = 0;
    if (decoder->fCodecContext->width == decoder->fVideoFrame->linesize[0])
    {
        memcpy(pucDataY, decoder->fVideoFrame->data[0], size);
    }
    else
    {
        BYTE *pcTmpData = decoder->fVideoFrame->data[0];
        for (i = 0; i < decoder->fCodecContext->height; i++) {
            memcpy(pucDataY,  pcTmpData, decoder->fCodecContext->width);
            pucDataY += decoder->fCodecContext->width;
            pcTmpData += decoder->fVideoFrame->linesize[0];
            
        }
    }

    int iHeight = decoder->fCodecContext->height / 2;
    int iWidth = decoder->fCodecContext->width / 2;
    if ((iWidth == decoder->fVideoFrame->linesize[1]) &&
        (iWidth == decoder->fVideoFrame->linesize[2]))
    {
        memcpy(pucDataU, decoder->fVideoFrame->data[1], size / 4);
        memcpy(pucDataV, decoder->fVideoFrame->data[2], size / 4);
    }
    else
    {
        BYTE *pcTmpData1 = decoder->fVideoFrame->data[1];
        BYTE *pcTmpData2 = decoder->fVideoFrame->data[2];
        for (i = 0; i < iHeight; i++) {
            memcpy(pucDataU, pcTmpData1, iWidth);
            memcpy(pucDataV, pcTmpData2, iWidth);
            pucDataU += iWidth;
            pucDataV += iWidth;
            pcTmpData1 += decoder->fVideoFrame->linesize[1];
            pcTmpData2 += decoder->fVideoFrame->linesize[2];
        }
    }

    decoder->mPictureData.dwLineSize[0] = decoder->fCodecContext->width;
    decoder->mPictureData.dwLineSize[1] = iWidth;
    decoder->mPictureData.dwLineSize[2] = iWidth;
 

    return;
}



int avc_decode2(Decoder* decoder, char* buf, int nalLen)
{
	byte_t* data = (byte_t*)buf;
	int frameSize = 0;
	//len = avcodec_decode_video2(pAVCodecCtx, pAVFrame, &got_picture,
	//			&mAVPacket);
	int ret = avcodec_decode_video1(decoder->fCodecContext, decoder->fVideoFrame, &frameSize, data, nalLen);
	if (ret <= 0) {
		return ret;
	}
	clock_t t1=clock();//从这开始计时
	int width  = decoder->fCodecContext->width;
	int height = decoder->fCodecContext->height;
	byte_t* y = decoder->fVideoFrame->data[0];
	byte_t* u = decoder->fVideoFrame->data[1];
	byte_t* v = decoder->fVideoFrame->data[2];
	
	Render_UpdateBuf(decoder);
	Render_RenderYUV(decoder, &decoder->mPictureData);
	clock_t t2=clock();//从这开始计时
	return ret;	
}


jint Java_com_qhcloud_video_H264Decoder_DecoderToRGB(JNIEnv* env, jobject thiz,jlong pDecoder, jbyteArray in, jint nalLen, jbyteArray out)
{
	pthread_mutex_lock(&lock);
	clock_t t1=clock();//从这开始计时
	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * Pixel= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);

	Decoder * decoder=(Decoder * )pDecoder; 
	jint flag;
	if (decoder) {
		flag = avc_decode(decoder, Buf, nalLen, Pixel);
	}
	
	clock_t t2 = clock();//到这结束

	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);

	pthread_mutex_unlock(&lock);
	//LOGD("########## decod time = %d", (t2 - t1)/1000);
	return 1;
} 

jint Java_com_qhcloud_video_H264Decoder_DecoderToSurfaceView(JNIEnv* env, jobject thiz,jlong pDecoder, jbyteArray in, jint nalLen)
{
	pthread_mutex_lock(&lock);
	clock_t t1=clock();//从这开始计时
	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);

	Decoder * decoder=(Decoder * )pDecoder; 
	jint flag;
	if (decoder) {
		flag = avc_decode2(decoder, Buf, nalLen);
	}
	
	clock_t t2 = clock();//到这结束

	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);

	pthread_mutex_unlock(&lock);
	//LOGD("########## decod time = %d", (t2 - t1)/1000);
	return 1;
}

 
static void cvt_420p_to_rgb565(int width, int height, const unsigned char *src, unsigned short *dst)
{
  int line, col, linewidth;
  int y, u, v, yy, vr, ug, vg, ub;
  int r, g, b;
  const unsigned char *py, *pu, *pv;

  linewidth = width >> 1;
  py = src;
  pu = py + (width * height);
  pv = pu + (width * height) / 4;

  y = *py++;
  yy = y << 8;
  u = *pu - 128;
  ug = 88 * u;
  ub = 454 * u;
  v = *pv - 128;
  vg = 183 * v;
  vr = 359 * v;

  for (line = 0; line < height; line++) {
    for (col = 0; col < width; col++) {
      r = (yy + vr) >> 8;
      g = (yy - ug - vg) >> 8;
      b = (yy + ub ) >> 8;

      if (r < 0) r = 0;
      if (r > 255) r = 255;
      if (g < 0) g = 0;
      if (g > 255) g = 255;
      if (b < 0) b = 0;
      if (b > 255) b = 255;
      *dst++ = (((unsigned short)r>>3)<<11) | (((unsigned short)g>>2)<<5) | (((unsigned short)b>>3)<<0);

      y = *py++;
      yy = y << 8;
      if (col & 1) {
    pu++;
    pv++;

    u = *pu - 128;
    ug = 88 * u;
    ub = 454 * u;
    v = *pv - 128;
    vg = 183 * v;
    vr = 359 * v;
      }
    }
    if ((line & 1) == 0) {
      pu -= linewidth;
      pv -= linewidth;
    }
  }
}

 jint Java_com_qhcloud_video_H264Decoder_DecoderToYUV(JNIEnv* env, jobject thiz,jlong pDecoder, jbyteArray in, jint nalLen, jbyteArray out)
{
		jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * Pixel= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	//jbyte * Pixel2= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	Decoder * decoder=(Decoder * )pDecoder; 
	jint flag;
	if (decoder) { 
		byte_t* data = (byte_t*)Buf;
		int frameSize = 0; 
		int ret = avcodec_decode_video1(decoder->fCodecContext, decoder->fVideoFrame, &frameSize, data, nalLen); 
		flag = avc_decode_get_stream(decoder, Pixel);
		//cvt_420p_to_rgb565(decoder->fDisplayWidth, decoder->fDisplayHeight, Pixel, Pixel2);
	}
	
	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);

	return 1;
} 

#ifdef __cplusplus
}
#endif