
#include <jni.h>
#include "Main.h"
#include <android/log.h>
#include "PullStream.h"
#include "FFDecode.h"
#define LOG_TAG "sanbot_decode"
#define Lg(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)

typedef struct streamInfo
{
    int streamId;
    int ch;
    int hardDecode;
    int justIframe;
} stream_info;

static stream_info streamInfos[2];

static JavaVM *g_jvm = 0;
jobject jobj = 0;

void Java_com_sanbot_decode_Decode_init(JNIEnv *env, jobject obj)
{

    if (g_jvm == 0)
    {
        (*env)->GetJavaVM(env, &g_jvm);
        //env->GetJavaVM(&g_jvm);
    }
    if (jobj == 0)
    {
        jobj = (*env)->NewGlobalRef(env, obj);
        //jobj = env->NewGlobalRef(obj);
    }
    //init streaminfos
    for (int i = 0; i < sizeof(streamInfos) / sizeof(streamInfos[0]); i++)
    {
        streamInfos[i].ch = -1;
        streamInfos[i].hardDecode = -1;
        streamInfos[i].justIframe = -1;
        streamInfos[i].streamId = -1;
    }

    StreamInit();
    ffInit();
}

void returnVideo(int streamId, char *data, int len, int width, int height)
{
    JNIEnv *env = 0;
    (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

    static jclass s_jc = 0;
    static jmethodID s_jm = 0;
    if (env != 0)
    {
        if (s_jc == 0)
        {
            s_jc = (*env)->GetObjectClass(env, jobj);
        }

        if (s_jm == 0)
        {
            s_jm = (*env)->GetMethodID(env, s_jc, "showVideo", "([BIIII)V");
        }

        if (s_jm != 0)
        {
            jbyteArray byteArray = (*env)->NewByteArray(env, len);
            (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)data);
            (*env)->CallVoidMethod(env, jobj, s_jm, byteArray, streamId, len, width, height);
            (*env)->DeleteLocalRef(env, byteArray);
        }
    }
    (*g_jvm)->DetachCurrentThread(g_jvm);
}

void streamCallBack(int handle, unsigned char *buf, int len, void *pUser)
{
    Lg("handle = %d,len = %d", handle, len);

    av_t *av_info = (av_t *)pUser;
    if (av_info->handle < 0)
    {
        Lg("get stream handle = %d", av_info->handle);
        return;
    }

    ptc_frame_head_t *frameInfo = (ptc_frame_head_t *)buf;
    int head_len = sizeof(ptc_frame_head_t);

    //jpeg frame , direct return
    if (frameInfo->channel_no == 2)
    {
        returnVideo(handle, (char *)buf + head_len, frameInfo->len, frameInfo->width, frameInfo->height);
        return;
    }

    int needDecode = 0;
    int justI = 0;
    for (int i = 0; i < sizeof(streamInfos); i++)
    {
        int streamId = streamInfos[i].streamId;
        // Lg("i = %d ; stream id = %d ; hardDecode = %d ; just = %d", i, streamId, streamInfos[i].hardDecode, streamInfos[i].justIframe);
        if (streamId == handle)
        {
            if (streamInfos[i].hardDecode == 1)
            {
                needDecode = 1;
            }
            if (streamInfos[i].justIframe == 1)
            {
                justI = 1;
            }
            break;
        }
    }

    if (!needDecode)
    {
        if (justI && frameInfo->frame_type != 1)
        {
            return;
        }
        returnVideo(handle, (char *)buf + head_len, frameInfo->len, frameInfo->width, frameInfo->height);
        return;
    }

    // start h264 to nv21 decode
    if (frameInfo->frame_type != 1 && justI)
    {
        return;
    }

    if (av_info->tempbuf != NULL)
    {
        // memset(av_info->tempbuf, 0, VIDEO_BUF_LEN);
        av_info->width = frameInfo->width;
        av_info->height = frameInfo->height;
        av_info->frame_sub_type = frameInfo->frame_sub_type;
        int Outsize = 0;
        Lg("开始解析视频 frame sub type = %d ; width = %d ; height = %d", frameInfo->frame_sub_type, av_info->width, av_info->height);
        decodeH264(av_info->width, av_info->height, (char *)buf + head_len, frameInfo->len, av_info->tempbuf, &Outsize);
        returnVideo(handle, (unsigned char *)av_info->tempbuf, Outsize, av_info->width, av_info->height);
    }
}

int Java_com_sanbot_decode_Decode_open(JNIEnv *env, jobject obj, jint ch, jint hardDecode, jint justIframe)
{
    int cch = ch;
    int chd = hardDecode;
    int cjif = justIframe;
    Lg("start open ch = %d ; hardDecode = %d ; justIframe = %d", cch, chd, cjif);
    int i = 0;
    for (; i < sizeof(streamInfos) / sizeof(streamInfos[0]); i++)
    {
        // Lg("stream index of i , streamId = %d ; hdc = %d ; justFrame = %d ; ch = %d ", streamInfos[i].streamId, streamInfos[i].hardDecode, streamInfos[i].justIframe, streamInfos[i].ch);
        if (i == sizeof(streamInfos) / sizeof(streamInfos[0]))
        {
            Lg("over pull stream num!");
            return -1;
        }
        if (streamInfos[i].streamId < 0)
        {
            break;
        }
    }
    int streamId = openStream(cch, chd, cjif, streamCallBack);
    if (streamId >= 0)
    {
        streamInfos[i].streamId = streamId;
        streamInfos[i].ch = cch;
        streamInfos[i].hardDecode = chd;
        streamInfos[i].justIframe = cjif;
    }
    // Lg("i = %d ; pull hd = %d ", i, streamInfos[i].hardDecode);
    return streamId;
}

int Java_com_sanbot_decode_Decode_close(JNIEnv *env, jobject obj, int streamId)
{
    if (streamId == -1)
    {
        Lg("close stream id = -1 , you don't have to close it");
        return -1;
    }
    closeStream(streamId);
}
