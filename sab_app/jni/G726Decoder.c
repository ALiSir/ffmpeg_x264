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
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR , "G726", __VA_ARGS__)

static int gs_index_adjust[8]= {-1,-1,-1,-1,2,4,6,8};
static int gs_step_table[89] = 
{
	7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,
	50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,
	408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,
	2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
	10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767
};

int m_nEnAudioPreSample = 0;
int m_nEnAudioIndex = 0;
int m_nDeAudioPreSample = 0;
int m_nDeAudioIndex = 0;
  
void EncoderClr(void)
{
	m_nEnAudioPreSample = 0;
	m_nEnAudioIndex = 0;
}

void DecoderClr(void)
{
	m_nDeAudioPreSample = 0;
	m_nDeAudioIndex = 0;
}

void ADPCMEncode(unsigned char *pRaw, int nLenRaw, unsigned char *pBufEncoded)
{
	short *pcm = (short *)pRaw;
	int cur_sample;
	int i;
	int delta;
	int sb;
	int code;
	nLenRaw >>= 1;

    EncoderClr();

	for(i = 0; i<nLenRaw; i++)
	{
		cur_sample = pcm[i]; 
		delta = cur_sample - m_nEnAudioPreSample;
		if (delta < 0){
			delta = -delta;
			sb = 8;
		}else sb = 0;

		code = 4 * delta / gs_step_table[m_nEnAudioIndex];	
		if (code>7)	code=7;

		delta = (gs_step_table[m_nEnAudioIndex] * code) / 4 + gs_step_table[m_nEnAudioIndex] / 8;
		if(sb) delta = -delta;

		m_nEnAudioPreSample += delta;
		if (m_nEnAudioPreSample > 32767) m_nEnAudioPreSample = 32767;
		else if (m_nEnAudioPreSample < -32768) m_nEnAudioPreSample = -32768;

		m_nEnAudioIndex += gs_index_adjust[code];
		if(m_nEnAudioIndex < 0) m_nEnAudioIndex = 0;
		else if(m_nEnAudioIndex > 88) m_nEnAudioIndex = 88;

		if(i & 0x01) pBufEncoded[i>>1] |= code | sb;
		else pBufEncoded[i>>1] = (code | sb) << 4;
	}
}

void ADPCMDecode(char *pDataCompressed, int nLenData, char *pDecoded)
{
	int i;
	int code;
	int sb;
	int delta;
	short *pcm = (short *)pDecoded;
	nLenData <<= 1;

    DecoderClr();

	for(i=0; i<nLenData; i++)
	{
		if(i & 0x01) code = pDataCompressed[i>>1] & 0x0f;
		else code = pDataCompressed[i>>1] >> 4;

		if((code & 8) != 0) sb = 1;
		else sb = 0;
		code &= 7;

		delta = (gs_step_table[m_nDeAudioIndex] * code) / 4 + gs_step_table[m_nDeAudioIndex] / 8;
		if(sb) delta = -delta;

		m_nDeAudioPreSample += delta;
		if(m_nDeAudioPreSample > 32767) m_nDeAudioPreSample = 32767;
		else if (m_nDeAudioPreSample < -32768) m_nDeAudioPreSample = -32768;

		pcm[i] = m_nDeAudioPreSample;
		m_nDeAudioIndex+= gs_index_adjust[code];
		if(m_nDeAudioIndex < 0) m_nDeAudioIndex = 0;
		if(m_nDeAudioIndex > 88) m_nDeAudioIndex= 88;
	}
}
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/HelloJni/HelloJni.java
 */ 
jint Java_com_audio_G726Decoder_DecoderG726Audio(JNIEnv* env, jobject thiz, jbyteArray in, jint nalLen, jbyteArray out)
{
	jbyte * Buf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * Pixel= (jbyte*)(*env)->GetByteArrayElements(env, out, 0);

	 ADPCMDecode(Buf, nalLen, Pixel);
	(*env)->ReleaseByteArrayElements(env, in, Buf, 0);
	(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);

	return 1;
} 
 
  