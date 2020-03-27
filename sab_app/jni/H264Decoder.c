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
#include <unistd.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <jpeglib/jpeglib.h>  
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR , "FFMPEG", __VA_ARGS__)
#include<pthread.h>
static pthread_mutex_t lock;

#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavutil/pixfmt.h>

#define RENDER_MAX_Y_SIZE 	(2600 * 1600 *3/2)
#define RENDER_MAX_UV_SIZE 	(RENDER_MAX_Y_SIZE/4)

typedef unsigned char byte_t; 
typedef unsigned char BYTE; 

static const char FRAG_SHADER[] = "varying lowp vec2 tc;\n"
"uniform sampler2D SamplerY;\n"
"uniform sampler2D SamplerU;\n"
"uniform sampler2D SamplerV;\n"
"void main(void)\n"
"{\n"
"mediump vec3 yuv;\n"
"lowp vec3 rgb;\n"
"yuv.x = texture2D(SamplerY, tc).r;\n"
"yuv.y = texture2D(SamplerU, tc).r - 0.5;\n"
"yuv.z = texture2D(SamplerV, tc).r - 0.5;\n"
"rgb = mat3( 1,   1,   1,\n"
"0,       -0.39465,  2.03211,\n"
"1.13983,   -0.58060,  0) * yuv;\n"
"gl_FragColor = vec4(rgb, 1);\n"
"}\n";
static const char VERTEX_SHADER[] = "attribute vec4 Position;    \n"
"attribute vec2 TexCoordIn;   \n"
"varying vec2 tc;     \n"
"void main()                  \n"
"{                            \n"
"   gl_Position = Position;  \n"
"   tc = TexCoordIn;  \n"
"}";

const GLfloat coordVertices_init[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
1.0f, };

#define RENDER_MAX_Y_SIZE 	(2600 * 1600 *3/2)
#define RENDER_MAX_UV_SIZE 	(RENDER_MAX_Y_SIZE/4)

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
GLfloat coordVertices[8];
unsigned long mViewportWidth;
unsigned long mViewportHeight;

GLuint mTextureY;
GLuint mTextureU;
GLuint mTextureV;
GLuint mSimpleProgram;
int mAttribVertex;
int mAttribTexture;
//CriticalSection mMutex;
int file_exist;
struct jpeg_compress_struct jpeg; 
struct jpeg_error_mgr jerr; 
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

void CheckGLError(const char* op) {
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGD("%s()  trigger glError (0x%x)\n", op, error);
    }
}

GLuint BuildShader(const char* source, GLenum shaderType) {
    GLuint shaderHandle = glCreateShader(shaderType);

    if (shaderHandle) {
		LOGD("BuildShader -> shaderHandle:%p line:%d source:%s\n", shaderHandle, __LINE__, source);
        glShaderSource(shaderHandle, 1, &source, 0);
		LOGD("BuildShader -> shaderHandle:%p line:%d\n", shaderHandle, __LINE__);
        glCompileShader(shaderHandle);
		LOGD("BuildShader -> shaderHandle:%p line:%d\n", shaderHandle, __LINE__);
        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }
		LOGD("BuildShader -> compiled:%p\n", compiled);
    } else {
		CheckGLError("glCreateShader");
	}
		LOGD("BuildShader -> shaderHandle:%p line:%d\n", shaderHandle, __LINE__);
    return shaderHandle;
}

GLuint BuildProgram(Decoder * decoder, const char* vertexShaderSource,
		const char* fragmentShaderSource) {
	LOGD("BuildProgram\n");
	GLuint vertexShader = BuildShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = BuildShader(fragmentShaderSource,
			GL_FRAGMENT_SHADER);
	GLuint programHandle = glCreateProgram();

	if (!vertexShader) {
		LOGD("vertexShader error\n");
	}
	if (!fragmentShader) {
		LOGD("fragmentShader error\n");
	}
	LOGD("programHandle:%d\n", programHandle);

	if (programHandle) {
		LOGD("programHandle OK\n");

		glAttachShader(programHandle, vertexShader);
		CheckGLError("glAttachShader");

		glAttachShader(programHandle, fragmentShader);
		CheckGLError("glAttachShader");

		glLinkProgram(programHandle);

		decoder->mAttribVertex = glGetAttribLocation(programHandle, "Position");
		decoder->mAttribTexture = glGetAttribLocation(programHandle, "TexCoordIn");

		// Galaxy Nexus 4.2.2
		static GLfloat squareVertices[] = { -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f, };

		glVertexAttribPointer(decoder->mAttribVertex, 2, GL_FLOAT, 0, 0, squareVertices);

		glEnableVertexAttribArray(decoder->mAttribVertex);

		glVertexAttribPointer(decoder->mAttribTexture, 2, GL_FLOAT, 0, 0, decoder->coordVertices);

		glEnableVertexAttribArray(decoder->mAttribTexture);

		GLint linkStatus = GL_FALSE;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
					//AIRIMOS_LOG_ERROR("error::Could not link program:\n%s\n",
							//buf);
					free(buf);
				}
			}
			glDeleteProgram(programHandle);
			programHandle = 0;
		}

	}

	return programHandle;
}



GLuint BindTexture(GLuint texture, const unsigned char *pucBuffer,
                                GLuint w, GLuint h) {
	glBindTexture(GL_TEXTURE_2D, texture);
	CheckGLError("glBindTexture");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, pucBuffer);
	CheckGLError("glTexImage2D");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CheckGLError("glTexParameteri");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGLError("glTexParameteri");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CheckGLError("glTexParameteri");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CheckGLError("glTexParameteri");

	return texture;
}

void RenderFrame(Decoder* decoder) {
    //LOGS();

    glClearColor(0.2f, 0.2f, 0.2f, 1);

    glClear (GL_COLOR_BUFFER_BIT);

    GLint tex_y = glGetUniformLocation(decoder->mSimpleProgram, "SamplerY");

    GLint tex_u = glGetUniformLocation(decoder->mSimpleProgram, "SamplerU");

    GLint tex_v = glGetUniformLocation(decoder->mSimpleProgram, "SamplerV");

    glActiveTexture (GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, decoder->mTextureY);

    glUniform1i(tex_y, 0);

    glActiveTexture (GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, decoder->mTextureU);

    glUniform1i(tex_u, 1);

    glActiveTexture (GL_TEXTURE2);

    glBindTexture(GL_TEXTURE_2D, decoder->mTextureV);

    glUniform1i(tex_v, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //mGotNewData = false;
    //SDKLOGV("+++++++++++RenderFramer+++++++++++++++");
}

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

jlong Java_com_qhcloud_video_H264Decoder_InitDecoder(JNIEnv* env, jobject thiz)
{
	pthread_mutex_lock(&lock);
	LOGD("Java_com_qhcloud_video_H264Decoder_InitDecoder");
	Decoder * decoder  = (Decoder *)av_malloc(sizeof(Decoder));

	//LOGD(str);
	//avcodec_init();

	avcodec_register_all();
	//av_register_all();

	decoder->fCodec = avcodec_find_decoder(CODEC_ID_H264);
	//decoder->fDisplayWidth  = width;
	//decoder->fDisplayHeight = height;
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

jlong Java_com_qhcloud_video_H264Decoder_SetVideoInfo(JNIEnv* env, jobject thiz, jlong pDecoder, jint width, jint height)
{
	pthread_mutex_lock(&lock);
	LOGD("Java_com_qhcloud_video_H264Decoder_SetVideoInfo");
	Decoder * decoder=(Decoder * )pDecoder; 
	
	if (decoder != NULL) {
		decoder->fDisplayWidth  = width;
		decoder->fDisplayHeight = height;
	}
	
	pthread_mutex_unlock(&lock);
	
	return 0;
	 
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
	/*int width  = decoder->fCodecContext->width;
	int height = decoder->fCodecContext->height;
	byte_t* y = decoder->fVideoFrame->data[0];
	byte_t* u = decoder->fVideoFrame->data[1];
	byte_t* v = decoder->fVideoFrame->data[2];
	*/
	//LOGD("decod video frameSize:%d", frameSize);
	Render_UpdateBuf(decoder);
	//Render_RenderYUV(decoder, &decoder->mPictureData);
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

 jint Java_com_qhcloud_video_H264Decoder_DecodeVideo(JNIEnv* env, jobject thiz,jlong pDecoder, jbyteArray in, jint nalLen)
{
	pthread_mutex_lock(&lock);
	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	//jbyte * Pixel= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	//jbyte * Pixel2= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	
	//LOGD("########## DecodeVideo.........");
	Decoder * decoder=(Decoder * )pDecoder; 
	jint flag;
	if (decoder) { 
		byte_t* data = (byte_t*)Buf;
		int frameSize = 0; 
		flag = avc_decode2(decoder, Buf, nalLen);
	}
 
 
	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	//(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);
	pthread_mutex_unlock(&lock);
	return 1;
} 

JNIEXPORT jint JNICALL Java_com_qhcloud_video_H264Decoder_RendererViewport(JNIEnv *env, jobject thiz, jlong pDecoder, jint ulWidth, jint ulHeight)
{
	LOGD("########## RendererViewport.........%llu", pDecoder);
	pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 
	if (decoder == NULL)
	{
		pthread_mutex_unlock(&lock);
		return -1;
	}
	decoder->mViewportWidth = ulWidth;
    decoder->mViewportHeight = ulHeight;

	LOGD("########## RendererViewport = width:%d height:%d", ulWidth, ulHeight);
	pthread_mutex_unlock(&lock);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_qhcloud_video_H264Decoder_RendererInit(JNIEnv *env, jobject thiz, jlong pDecoder)
{
	LOGD("########## RendererInit......1...");
    pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 
	if (decoder == NULL)
	{
		pthread_mutex_unlock(&lock);
		return -1;
	}

    memcpy(decoder->coordVertices, coordVertices_init, sizeof(decoder->coordVertices));	
	decoder->mSimpleProgram = BuildProgram(decoder, VERTEX_SHADER, FRAG_SHADER);

    glUseProgram(decoder->mSimpleProgram);

    glGenTextures(1, &decoder->mTextureY);
    glGenTextures(1, &decoder->mTextureU);
    glGenTextures(1, &decoder->mTextureV);
	LOGD("########## RendererInit......2...");
	pthread_mutex_unlock(&lock);
	return 0;
}



JNIEXPORT jint JNICALL Java_com_qhcloud_video_H264Decoder_RenderVideo(JNIEnv *env, jobject thiz, jlong pDecoder)
{
    pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 
	if (decoder == NULL)
	{
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	unsigned char *yBuffer, *uBuffer, *vBuffer;
	unsigned long ulWidth;
	unsigned long ulHeight; 

	if (0 == decoder->mPictureData.dwPicWidth || 0 == decoder->mPictureData.dwPicHeight) {
		LOGD(" width or height is zero");
		pthread_mutex_unlock(&lock);
		return 0;
	}

	//CritScope lock(&mMutex);

	yBuffer = (unsigned char *) decoder->mPictureData.pucData[0];
	uBuffer = (unsigned char *) decoder->mPictureData.pucData[1];
	vBuffer = (unsigned char *) decoder->mPictureData.pucData[2];

	ulWidth = decoder->mPictureData.dwPicWidth;
	ulHeight = decoder->mPictureData.dwPicHeight;

	//SetViewport(ulWidth, ulHeight);
	glViewport(0, 0, decoder->mViewportWidth, decoder->mViewportHeight);

	//   mScaleRatio=0.2f;
	//   Scale(mScaleRatio);

	BindTexture(decoder->mTextureY, yBuffer, ulWidth, ulHeight);
	BindTexture(decoder->mTextureU, uBuffer, ulWidth / 2, ulHeight / 2);
	BindTexture(decoder->mTextureV, vBuffer, ulWidth / 2, ulHeight / 2);
	usleep(40000);
	RenderFrame(decoder);
	
	/*if (decoder->file_exist != 1) {
		decoder->file_exist = 1;
		int width = decoder->mPictureData.dwPicWidth;
	    int height= decoder->mPictureData.dwPicHeight;
		int len = 3 * width * height;//pstPictureData->dwLineSize[0] * height+pstPictureData->dwLineSize[1] * height / 4+ pstPictureData->dwLineSize[2] * height / 4 ;
		//YV12 ： 亮度（行×列） ＋ U（行×列/4) + V（行×列/4）
		///storage/emulated/0/qlink/data.yuv
		unsigned char* DisplayBuf = NULL;
 		int p;
		int i;
 		if (DisplayBuf == NULL) {
        	DisplayBuf = (unsigned char *)malloc(sizeof(unsigned char)*(len));
        }
		memset(DisplayBuf,0,len);
		 p=0;
		for(i=0; i<height; i++)
		{ 
			memcpy(DisplayBuf+p,decoder->mPictureData.pucData[0] + i * decoder->mPictureData.dwLineSize[0], width);
			p+=width;
		} 

 		for(i=0; i<height/2; i++)
		{
			memcpy(DisplayBuf+p,decoder->mPictureData.pucData[1] + i * decoder->mPictureData.dwLineSize[1], width/2);
			p+=width/2;
		} 
		
		for(i=0; i<height/2; i++)
		{
			memcpy(DisplayBuf+p,decoder->mPictureData.pucData[2] + i * decoder->mPictureData.dwLineSize[2], width/2);
			p+=width/2;
		} 

		if (remove("storage/emulated/0/qlink/data.yuv") == 0) {
		}
		FILE* file = fopen("storage/emulated/0/qlink/data.yuv", "wb");
		if (file != NULL) {
			fwrite(DisplayBuf, len, 1, file);
			fclose(file);
		}
		
		free(DisplayBuf);
	}*/
	
	pthread_mutex_unlock(&lock);
	
	//LOGD("########## RenderVideo....ulWidth:%d ulHeight:%d.....decoder->mTextureY:%p decoder->mTextureU:%p decoder->mTextureV:%p yBuffer:%p uBuffer:%p vBuffer:%p", ulWidth, ulHeight, decoder->mTextureY, decoder->mTextureU, decoder->mTextureV, yBuffer, uBuffer, vBuffer);
	return 1; 
}

int SaveFrame(int nszBuffer, uint8_t *buffer, char cOutFileName[])
{
	//printf("SaveFrame nszBuffer = %d, cOutFileName = %s\n", nszBuffer, cOutFileName);
	int iRet = 0;

	if( nszBuffer > 0 )
	{
	FILE *pFile = pFile = fopen(cOutFileName, "wb");
	if(pFile)
	{
		fwrite(buffer, sizeof(uint8_t), nszBuffer, pFile);
		iRet = 1;
		fclose(pFile);
	}
	}
   return iRet;
}


int WriteJPEG (Decoder * decoder, AVCodecContext *pCodecCtx, AVFrame *pFrame, char cFileName[], uint8_t *buffer, int numBytes)
{ 
	int iRet = 0;
	
   AVCodec *pMJPEGCodec=NULL;
   AVCodecContext *pMJPEGCtx = avcodec_alloc_context3(decoder->fCodec);
   if( pMJPEGCtx )
   {
      pMJPEGCtx->bit_rate = pCodecCtx->bit_rate;
      pMJPEGCtx->width = pCodecCtx->width;
      pMJPEGCtx->height = pCodecCtx->height;
      pMJPEGCtx->pix_fmt = PIX_FMT_YUVJ420P;
      pMJPEGCtx->codec_id = CODEC_ID_MJPEG;
      pMJPEGCtx->codec_type = AVMEDIA_TYPE_VIDEO;
      pMJPEGCtx->time_base.num = pCodecCtx->time_base.num;
      pMJPEGCtx->time_base.den = pCodecCtx->time_base.den;
      pMJPEGCodec = avcodec_find_encoder(pMJPEGCtx->codec_id );

	  LOGD("########## WriteJPEG.........iRet:%d pMJPEGCodec:%p line:%d", iRet, pMJPEGCodec, __LINE__);
      if( pMJPEGCodec && (avcodec_open2( pMJPEGCtx, pMJPEGCodec, NULL) >= 0) )
      {
         pMJPEGCtx->qmin = pMJPEGCtx->qmax = 3;
         pMJPEGCtx->mb_lmin = pMJPEGCtx->lmin = pMJPEGCtx->qmin * FF_QP2LAMBDA;
         pMJPEGCtx->mb_lmax = pMJPEGCtx->lmax = pMJPEGCtx->qmax * FF_QP2LAMBDA;
         pMJPEGCtx->flags |= CODEC_FLAG_QSCALE;
         pFrame->quality = 10;
         pFrame->pts = 0;
         int szBufferActual = avcodec_encode_video(pMJPEGCtx, buffer, numBytes, pFrame);
            
         if( SaveFrame(szBufferActual, buffer, cFileName ) )
            iRet = 1;
			LOGD("########## WriteJPEG.........iRet:%d line:%d", iRet, __LINE__);
         avcodec_close(pMJPEGCtx);
      }
   }
   return iRet;
} 

#define JPEG_QUALITY 100 //图片质量  
  
int savejpg(unsigned char *pdata, char *jpg_file, int width, int height)  
{  //分别为RGB数据，要保存的jpg文件名，图片长宽  
    int depth = 3;  
    JSAMPROW row_pointer[1];//指向一行图像数据的指针  
    struct jpeg_compress_struct cinfo;  
    struct jpeg_error_mgr jerr;  
    FILE *outfile;  
  
    cinfo.err = jpeg_std_error(&jerr);//要首先初始化错误信息  
    //* Now we can initialize the JPEG compression object.  
    jpeg_create_compress(&cinfo);  
  
    if ((outfile = fopen(jpg_file, "wb")) == NULL)  
    {  
        fprintf(stderr, "can't open %s\n", jpg_file);  
        return -1;  
    }  
    jpeg_stdio_dest(&cinfo, outfile);  
  
    cinfo.image_width = width;             //* image width and height, in pixels  
    cinfo.image_height = height;  
    cinfo.input_components = depth;    //* # of color components per pixel  
    cinfo.in_color_space = JCS_RGB;     //* colorspace of input image  
    jpeg_set_defaults(&cinfo);  
  
    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE ); //* limit to baseline-JPEG values  
    jpeg_start_compress(&cinfo, TRUE);  
  
    int row_stride = width * 3;  
    while (cinfo.next_scanline < cinfo.image_height)  
           {  
            row_pointer[0] = (JSAMPROW)(pdata + cinfo.next_scanline * row_stride);//一行一行数据的传，jpeg为大端数据格式  
            jpeg_write_scanlines(&cinfo, row_pointer, 1);  
        }  
  
    jpeg_finish_compress(&cinfo);  
    jpeg_destroy_compress(&cinfo);//这几个函数都是固定流程  
    fclose(outfile);  
  
    return 0;  
}  

int jpeginit(Decoder * decoder, int image_width,int image_height,int quality)  
{  
      
	LOGD("jpeginit.... LINE:%d jerr:%p", __LINE__, decoder->jerr);
    //错误输出在绑定  
    decoder->jpeg.err = jpeg_std_error(&decoder->jerr);  
	LOGD("jpeginit.... LINE:%d", __LINE__);
    //初始化压缩对象  
    jpeg_create_compress(&decoder->jpeg);  
	LOGD("jpeginit.... LINE:%d", __LINE__);
    //压缩参数设置。具体请到网上找相应的文档吧，参数很多，这里只设置主要的。  
    //我设置为一个 24 位的 image_width　X　image_height大小的ＲＧＢ图片  
    decoder->jpeg.image_width = image_width;  
    decoder->jpeg.image_height = image_height;  
    decoder->jpeg.input_components  = 3;  
    decoder->jpeg.in_color_space = JCS_RGB;  
    //参数设置为默认的  
    jpeg_set_defaults(&decoder->jpeg);  
    //还可以设置些其他参数：  
    //// 指定亮度及色度质量  
    decoder->jpeg.q_scale_factor[0] = jpeg_quality_scaling(100);  
    decoder->jpeg.q_scale_factor[1] = jpeg_quality_scaling(100);  
    //// 图像采样率，默认为2 * 2  
    decoder->jpeg.comp_info[0].v_samp_factor = 1;  
    decoder->jpeg.comp_info[0].h_samp_factor = 1;  
    //// 设定编码jpeg压缩质量  
	LOGD("jpeginit.... LINE:%d", __LINE__);
   // jpeg_set_quality(&jpeg, quality, TRUE);  
     
    return 0;  
}  
  
int rgb2jpeg(Decoder * decoder, char * filename, unsigned char* rgbData)  
{
    //定义压缩后的输出，这里输出到一个文件！  
    FILE* pFile = fopen( filename,"wb" );  
    if( !pFile )  
        return 0;  
    //绑定输出  
    jpeg_stdio_dest(&decoder->jpeg, pFile);  
      
    //开始压缩。执行这一行数据后，无法再设置参数了！  
    jpeg_start_compress(&decoder->jpeg, TRUE);  
      
    JSAMPROW row_pointer[1];  
    //从上到下，设置图片中每一行的像素值  
	int i;
    for(i=0;i<decoder->jpeg.image_height;i++ )  
    {  
        row_pointer[0] = rgbData+i*decoder->jpeg.image_width*3;  
        jpeg_write_scanlines( &decoder->jpeg,row_pointer,1 );  
    }  
    //结束压缩  
    jpeg_finish_compress(&decoder->jpeg);  
  
      
    fclose( pFile );  
    pFile = NULL;  
    return 0;  
      
}  
int jpeguninit(Decoder * decoder)  
{  
    //清空对象  
    jpeg_destroy_compress(&decoder->jpeg);  
    return 0;  
}  

JNIEXPORT jint JNICALL Java_com_qhcloud_video_H264Decoder_CaptureFile(JNIEnv *env, jobject thiz, jlong pDecoder, jbyteArray in, jint nalLen, jstring filename)
{
	
    pthread_mutex_lock(&lock);
	Decoder * decoder=(Decoder * )pDecoder; 
	if (decoder == NULL)
	{
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	LOGD("########## CaptureFile........pDecoder:%p", decoder);
	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	const char *jfilename = (*env)->GetStringUTFChars(env, filename, 0);
	byte_t* data = (byte_t*)Buf;   
	int frameSize = 0; 
	LOGD("########## CaptureFile........pDecoder:%p nalLen:%d", decoder, nalLen);
 
	clock_t t1=clock();//从这开始计时
	int width  = decoder->fCodecContext->width;
	int height = decoder->fCodecContext->height;
	 
	byte_t* y = decoder->fVideoFrame->data[0];
	byte_t* u = decoder->fVideoFrame->data[1];
	byte_t* v = decoder->fVideoFrame->data[2];
	unsigned char* out = (unsigned char*)malloc(width*height*4);
	yuv420_2_rgb565(out, y, u, v, width, height, decoder->fVideoFrame->linesize[0], decoder->fVideoFrame->linesize[1], width << 1, yuv2rgb565_table,3);

	LOGD("########## CaptureFile...222.....pDecoder:%p nalLen:%d", decoder, nalLen);
	jpeginit(decoder, width, height, 10);
	LOGD("########## CaptureFile..jpeginit.......width:%d, height:%d out:%p", width, height, out);
	rgb2jpeg(decoder, jfilename, out);
	LOGD("########## CaptureFile..jpeginit.......width:%d, height:%d", width, height);
	jpeguninit(decoder);	
	//int iret = savejpg(out, (char*)jfilename, width, height);
	
	//LOGD("########## CaptureFile.........iret:%d jfilename:%s", iret, jfilename);
    (*env)->ReleaseStringUTFChars(env, filename, jfilename);
	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	free(out);
	pthread_mutex_unlock(&lock);
	return 0;//iret;
}
