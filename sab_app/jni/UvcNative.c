// #define _GNU_SOURCE
// #include <sched.h>
// #include <unistd.h> /* sysconf */
// #include <stdlib.h> /* exit */
// #include <stdio.h>


#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
 
#define TAG "Affinity"
#define DEBUG 1
 
#ifndef CPU_ZERO
#define CPU_SETSIZE 1024
#define __NCPUBITS  (8 * sizeof (unsigned long))
typedef struct
{
    unsigned long __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;
 
#define CPU_SET(cpu, cpusetp) \
  ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) \
  memset((cpusetp), 0, sizeof(cpu_set_t))
#else
#define CPU_SET(cpu,cpustep) ((void)0)
#define CPU_ZERO(cpu,cpustep) ((void)0)
#endif
 
// #ifdef DEBUG
// #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
// #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
// #else
// #define LOGD(...) ((void)0)
// #define LOGE(...) ((void)0)

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "UvcNative.h"
#include "alog.h"
#include "G711.h"
#include <pthread.h>
#include "playlib.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>  
#include <netinet/in.h>
#include <poll.h>


/*static int isHardDecode = 0;
static int isJastIframe = 0;*/

//异常捕 -- start --
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>

sigjmp_buf JUMP_ANCHOR;
volatile sig_atomic_t error_cnt = 0;

void exception_handler(int errorCode)
{
    error_cnt += 1;
    LOGI("JNI_ERROR face_service hfisone face_filter, error code %d, cnt %d", errorCode, error_cnt);
    siglongjmp(JUMP_ANCHOR, 1);
}

void exception_catch()
{
    // 注册要捕捉的系统信号量
    struct sigaction sigact;
    struct sigaction old_action;
    sigaction(SIGABRT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigset_t block_mask;
        sigemptyset(&block_mask);
        sigaddset(&block_mask, SIGABRT); // handler处理捕捉到的信号量时，需要阻塞的信号6
        sigaddset(&block_mask, SIGSEGV); // handler处理捕捉到的信号量时，需要阻塞的信号11
        sigaddset(&block_mask, SIGBUS);//信号量7

        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigact.sa_mask = block_mask;
        sigact.sa_handler = exception_handler;
        sigaction(SIGABRT, &sigact, NULL); // 注册要捕捉的信号6
        sigaction(SIGSEGV, &sigact, NULL); // 注册要捕捉的信号11
        sigaction(SIGBUS, &sigact, NULL); //信号量7
    }
}

//异常捕捉 --- end ---

static JavaVM *g_jvm = 0;
jobject jobj = 0;
jmethodID jmid = 0, jmid1 = 0;
jclass jcls = 0;

long getCurrentTime();

static int pFrameNum = -1;

static int uid = -1;

typedef struct
{
    unsigned int len;
    unsigned int no;
    unsigned int sec;
    unsigned int usec;
    unsigned short width;
    unsigned short height;
    unsigned char frame_type;     // 1=I, 2=P, 3=A, 255=录像结束帿 PTC_FRAME_E
    unsigned char frame_sub_type; //frame_type=1/2: PTC_VIDEO_ENCODE_E PTC_AUDIO_ENCODE_E
    unsigned char frame_rate;
    unsigned char security;
    unsigned char padding_bytes;
    unsigned char channel_type; //通道类型 PTC_CH_TYPE_E
    unsigned char channel_no;   //从零弿姿
    unsigned char reserve[1];
    unsigned int flag; //PTC_FRAME_FLAG
} ptc_frame_head_t;

#define AUDIO_IN_SECOND 2 * 8192 + 100
#define AUDIO_BUF_LEN 1024 * 8        //音频帧缓冲区大小
#define VIDEO_BUF_LEN 4 * 1024 * 1024 //视频帧缓冲区大小

#define AVCODEC_NUM 2

typedef struct avcodec_jni
{

    int handle;
    int audio_tick;
    int audio_len;

    char *audiobuf; //音频buf

    long vd_handle; //解码结构体地坿
    char *tempbuf;  //解码后视频帧数据指针

    int frame_sub_type;
    int width;
    int height;

    int hardDecode;
    int justIframe;

} av_t;

av_t av[AVCODEC_NUM];

pthread_t tickpid;
static int tickFlag = 0;
int tick;

void *tickThread()
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" tickThread 异常！");
        return NULL;
    }
    while (tickFlag)
    {
        sleep(1);
        //usleep(1000*1000);
        tick++;
    }
    return NULL;
}

typedef struct
{
    int cmd; //1:弿视频,2:关视颿,3心跳
    int ch;  //0:主码浿,1:子码浿
    int res; //0:成功,1:失败
} pd_msg_t;

typedef struct
{
    int used;
    int handle;

    int stream;
    int sock;

    int status;

    int pid_flag;
    pthread_t pid;

    char *buffer;

    void *param;
} pd_manager;

#define SERVER_PORT 5500
#define PG_MANGER_NUM 2
#define NET_VIDEO_BUF 800 * 1024

pd_manager mPdManager[PG_MANGER_NUM];

void CloseStream(int id)
{

    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" close stream 异常！");
        return;
    }
    exception_catch();

    LOGE("CloseStream = %d ", id);
    if (id < 0 || id > PG_MANGER_NUM)
        return;

    if (mPdManager[id].status == -1)
        return;

    pd_msg_t msg;
    msg.cmd = 2;
    msg.ch = mPdManager[id].stream;
    int ret = write(mPdManager[id].sock, &msg, sizeof(pd_msg_t));
    LOGE("write = %d %d", ret, mPdManager[id].sock);

    mPdManager[id].pid_flag = 0;
    mPdManager[id].used = 0;
    usleep(800 * 1000); //150ms

    LOGE("CloseStream end = 1 ");
    close(mPdManager[id].sock);

    LOGE("CloseStream end = 2 ");
    shutdown(mPdManager[id].sock, 2);

    LOGE("CloseStream end = 3 ");

    if (NULL == mPdManager[id].buffer)
    {

        LOGE("CloseStream end = 4 ");
        free(mPdManager[id].buffer);
        mPdManager[id].buffer = NULL;
        LOGE("CloseStream end = 5 ");
    }

    LOGE("CloseStream end = 6 ");
    mPdManager[id].status = -1;
    LOGE("CloseStream end = %d ", id);
}

int getManagerUnused()
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" getManagerUnused 异常！");
        return -1;
    }
    exception_catch();

    int i = 0;
    for (i = 0; i < PG_MANGER_NUM; i++)
    {
        if (mPdManager[i].used == 0)
            break;
        else
            continue;
    }
    if (i == PG_MANGER_NUM)
        return -1;
    else
        return i;
}

static int Recv(int fd, char *vptr, int n, int flag)
{

    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" Recv 异常！");
        return -1;
    }
    exception_catch();

    int nleft, nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nread = recv(fd, ptr, nleft, flag)) < 0)
        {
            return (-1);
        }
        else if (nread == 0)
        {
            break; //EOF
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

// void cpuSet();
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Init(JNIEnv *env, jobject obj)
{

    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" java init jni 异常！");
        return -1;
    }
    exception_catch();

    // cpuSet();

    ffmpeg_init();
    memset(&av, 0, AVCODEC_NUM * sizeof(av_t));
    int i = 0;
    for (i = 0; i < AVCODEC_NUM; i++)
    {
        av[i].handle = -1;
    }
    //tickFlag = 1;
    ///pthread_create(&tickpid,NULL,(void*)tickThread,NULL);
}

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Done(JNIEnv *env, jobject obj)
{
    //tickFlag = 0;
    //pthread_join(tickpid,NULL);
}

//设置解码方式
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetHardDecode(JNIEnv *env, jobject obj, int handle, int type)
{

    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" java SetHardDecode jni 异常！");
        return -1;
    }
    exception_catch();

    av_t *av_info;
    int i = 0;
    for (i = 0; i < AVCODEC_NUM; i++)
    {
        if (av[i].handle == handle)
            break;
    }
    if (i >= AVCODEC_NUM)
        return -1;
    else
        av_info = &av[i];

    av_info->hardDecode = type;
}

//解析的帧的类垿 I房 P
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetJastIframe(JNIEnv *env, jobject obj, int handle, int type)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" java SetJastIframe jni 异常！");
        return -1;
    }
    exception_catch();

    av_t *av_info;
    int i = 0;
    for (i = 0; i < AVCODEC_NUM; i++)
    {
        if (av[i].handle == handle)
            break;
    }
    if (i >= AVCODEC_NUM)
        return -1;
    else
        av_info = &av[i];

    av_info->justIframe = type;
}

//主码浿0 还是子码浿1 1280*720 还是640*480
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Open(JNIEnv *env, jobject obj, jint channel, int hardDecode, int justIframe)
{

    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" java Open jni 异常！");
        return -1;
    }

    exception_catch();
    //uid = qh_cam_open();

    //清理异常
    // (*env)->ExceptionClear(env);

    //crash test demo
    // char* a = NULL;
    // int val1 = a[1] - '0';

    // char* b = NULL;
    // int val2 = b[1] - '0';

    // LOGE("val 1 %d", val1);
    // LOGE(" test_i_1_1 = %d ",val1/val2);

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

    av_t *av_info;
    int i = 0;
    for (i = 0; i < AVCODEC_NUM; i++)
    {
        if (av[i].handle < 0)
            break;
    }
    if (i >= AVCODEC_NUM)
    {
        LOGI(" open Stream error,reason i >= AVCODEC_NUM current stream num is max！ i = %d ", i);
        return -1;
    }
    else
        av_info = &av[i];
    LOGE("open  %d ", i);
    if (av_info->audiobuf)
    {
        free(av_info->audiobuf);
        av_info->audiobuf = NULL;
    }
    if (av_info->tempbuf)
    {
        free(av_info->tempbuf);
        av_info->tempbuf = NULL;
    }

    av_info->audiobuf = (char *)malloc(AUDIO_BUF_LEN);
    memset(av_info->audiobuf, 0, AUDIO_BUF_LEN);
    av_info->tempbuf = (char *)malloc(VIDEO_BUF_LEN);
    memset(av_info->tempbuf, 0, VIDEO_BUF_LEN);
    //LOGE("分配空间吿 tempbuf 大小：tempbut = %d ", av_info->tempbuf);
    av_info->vd_handle = 0;

    int handle = OpenStream(channel, av_info);
    LOGE("open video , handle = %d", handle);
    if (handle < 0)
    {
        if (av_info->audiobuf)
            free(av_info->audiobuf);
        if (av_info->tempbuf)
            free(av_info->tempbuf);
        av_info->audiobuf = NULL;
        av_info->tempbuf = NULL;
        return -1;
    }
    av_info->handle = handle;
    av_info->hardDecode = hardDecode;
    av_info->justIframe = justIframe;

    LOGE("start handle = %d", handle);

    return handle;
}

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetPFrameNum(JNIEnv *env, jobject obj, int num)
{
    pFrameNum = num;
    return pFrameNum;
}

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Close(JNIEnv *env, jobject obj, jint handle)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" java Close jni 异常！");
        return -1;
    }
    exception_catch();

    LOGE("close handle = %d", handle);
    CloseStream((int)handle);
    av_t *av_info;
    int i = 0;
    for (i = 0; i < AVCODEC_NUM; i++)
    {
        if (av[i].handle == handle)
            break;
    }
    if (i >= AVCODEC_NUM)
        return -1;
    else
        av_info = &av[i];

    av_info->handle = -1;

    //qh_cam_stoppreview(handle);
    //qh_cam_close();
    /*if(fd != NULL)
     fclose(fd);*/

    video_ended(&av_info->vd_handle);
    av_info->vd_handle = 0;

    if (av_info->audiobuf != NULL)
        free(av_info->audiobuf);
    if (av_info->tempbuf != NULL)
        free(av_info->tempbuf);
    av_info->audiobuf = NULL;
    av_info->tempbuf = NULL;
    LOGE("close success handle = %d", handle);
    return 0;
}

void ShowVideo(int handle, char *data, int len, int width, int height, int raw_len, int frame_num, int status)
{
  
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" ShowVideo 异常！");
        return;
    }
    exception_catch();

    // LOGE("start return video stream => handle = %d len = %d",handle,len);
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
            s_jm = (*env)->GetMethodID(env, s_jc, "showVideo", "(I[BIIIII)V");
        }

        if (s_jm != 0)
        {
            jbyteArray byteArray = (*env)->NewByteArray(env, len);
            (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)data);
            // LOGE("return video stream => handle = %d  len = %d width = %d  height = %d ",handle,len,width,height);
            (*env)->CallVoidMethod(env, jobj, s_jm, handle, byteArray, width, height, raw_len, frame_num, status);
            (*env)->DeleteLocalRef(env, byteArray);
        }
    }
    (*g_jvm)->DetachCurrentThread(g_jvm);
    // LOGE("end return video stream => handle = %d len = %d",handle,len);

}

// typedef unsigned long int __cpu_mask;

// typedef struct
// {
//   __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
// } cpu_set_t;


// int cpuSet1(void)
// {
//     int i, nrcpus;
//     cpu_set_t mask;
//     unsigned long bitmask = 0;
    
//     CPU_ZERO(&mask);
    
//     CPU_SET(0, &mask); /* add CPU0 to cpu set */
//     CPU_SET(1, &mask); /* add CPU2 to cpu set */
// 	CPU_SET(2, &mask); /* add CPU2 to cpu set */
// 	CPU_SET(3, &mask); /* add CPU2 to cpu set */
// 	CPU_SET(4, &mask); /* add CPU2 to cpu set */
// 	CPU_SET(5, &mask); /* add CPU2 to cpu set */

//       /* Set the CPU affinity for a pid */
//     if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) 
//     {   
//         perror("sched_setaffinity");
//         exit(EXIT_FAILURE);
//     }
    
//     CPU_ZERO(&mask);
    
//      /* Get the CPU affinity for a pid */
//     if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1) 
//     {   
//         perror("sched_getaffinity");
//         exit(EXIT_FAILURE);
//     }

//        /* get logical cpu number */
//     nrcpus = sysconf(_SC_NPROCESSORS_CONF);
    
//     for (i = 0; i < nrcpus; i++)
//     {
//         if (CPU_ISSET(i, &mask))
//         {
//             bitmask |= (unsigned long)0x01 << i;
//             printf("processor #%d is set\n", i); 
//         }
//     }
//     printf("bitmask = %#lx\n", bitmask);

//     exit(EXIT_SUCCESS);
// }

void ShowAudio(int handle, char *data, int len, int type)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" ShowAudio 异常！");
        return;
    }
    exception_catch();

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
            s_jm = (*env)->GetMethodID(env, s_jc, "showAudio", "(I[BII)V");
        }
        if (s_jm != 0)
        {
            jbyteArray byteArray = (*env)->NewByteArray(env, len);
            (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)data);
            (*env)->CallVoidMethod(env, jobj, s_jm, (jint)handle, byteArray, len, type);
            (*env)->DeleteLocalRef(env, byteArray);
        }
    }
    (*g_jvm)->DetachCurrentThread(g_jvm);
}
int pFrameNo = 0;
int showTimeInfo = 0;
void LiveDataCallback2(int handle, unsigned char *data, int len, void *pUser)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" LiveDataCallback2 异常！");
        return;
    }
    exception_catch();

    av_t *av_info = (av_t *)pUser;
    if (av_info->handle < 0)
        return;

    ptc_frame_head_t *frameInfo = (ptc_frame_head_t *)data;
    int head_len = sizeof(ptc_frame_head_t);

    //    if(frameInfo->frame_type == 1||frameInfo->frame_type == 2)

    /*struct timeval tv_start; 
    struct timeval tv_end; 
    gettimeofday(&tv_start, NULL);



    if (frameInfo->frame_type != 3)*/
    if (showTimeInfo)
    {
        LOGE("jni_start get_stream sec = %d usec = %d no = %d", frameInfo->sec, frameInfo->usec, frameInfo->no);
    }

    //    return;
    if (frameInfo->frame_type == 1 || frameInfo->frame_type == 2)
    {
        //        if(frameInfo->frame_type == 1)
        // LOGE("准备解码＿ handle = %d len = %d frame_type= %d hard %d just %d",handle,len,frameInfo->frame_type,av_info->hardDecode,av_info->justIframe);
        if (av_info->hardDecode == 1)
        {
            if (av_info->justIframe && frameInfo->frame_type != 1)
            {
                return;
            }
            //            if(frameInfo->frame_type == 1 || frameInfo->frame_type == 2)
            //            {
            //   LOGE("handle = %0x %0x %0x %0x %0x",*(data+head_len),*(data+head_len+1),*(data+head_len+2),*(data+head_len+3),*(data+head_len+4),*(data+head_len+5));
            // LOGE("handle = %d len = %d",handle,len);
            ShowVideo(handle, (unsigned char *)data + head_len, frameInfo->len, frameInfo->width, frameInfo->height, 0, frameInfo->no, 0); //硬解砿
                                                                                                                                           //  LOGE("sec = %d usec = %d",frameInfo->sec,frameInfo->usec);
                                                                                                                                           //            }
        }
        else
        {
            if (frameInfo->frame_type == 1)
            {
                pFrameNo = 0;
            }
            pFrameNo++;
            //限制解码数据帧
            if (pFrameNum > 0 && pFrameNo > pFrameNum)
                return;

            // LOGE("弿始软解码：handle = %d len = %d",handle,len);
            if (av_info->justIframe && frameInfo->frame_type != 1)
            {
                return;
            }
            //  LOGE("aaaahandle3 = %d len = %d",handle,len);
            int beganFlag = 0;
            int Outsize = 0;
            if (av_info->tempbuf != NULL)
            {
                //  LOGE("aaaahandle4 = %d len = %d width= %d height= %d",handle,len,frameInfo->width,frameInfo->height);
                if ((av_info->vd_handle == 0) || (av_info->frame_sub_type != frameInfo->frame_sub_type) || (av_info->width != frameInfo->width) || (av_info->height != frameInfo->height))
                {
                    //  LOGE("aaaahandle5 = %d len = %d",handle,len);
                    if (av_info->vd_handle != 0)
                        video_ended(&av_info->vd_handle);
                    beganFlag = 1;
                }

                // LOGE("aaaahandle6 = %d len = %d",handle,len);
                long begin_time = getCurrentTime();
                if (beganFlag == 1 && video_began(av_info->hardDecode, frameInfo->width, frameInfo->height, &av_info->vd_handle) < 0)
                {
                    //    LOGE("aaaahandle7 = %d len = %d",handle,len);
                    //av_info->Iframe_flag = 0;
                    video_ended(&av_info->vd_handle);
                    return;
                }
                long begin_consuming = getCurrentTime()-begin_time;

                //   LOGE("aaaahandle8 = %d len = %d tempbuf = %d",handle,len,av_info->tempbuf);
                memset(av_info->tempbuf, 0, VIDEO_BUF_LEN);
                //av_info->frame_sub_type!=frameInfo->frame_sub_type;
                av_info->width = frameInfo->width;
                av_info->height = frameInfo->height;
                av_info->frame_sub_type = frameInfo->frame_sub_type;

                //   LOGE("av_info->handle = %d",av_info->handle);

                //LOGE("aaaahandle9 = %d len = %d width = %d height = %d type = %d",handle,len,frameInfo->width,frameInfo->height,frameInfo->frame_type);
                // LOGE("jni_codec_start = %d",frameInfo->no);

                if (showTimeInfo)
                {
                    LOGE("jni_start start_decode sec = %d usec = %d no = %d", frameInfo->sec, frameInfo->usec, frameInfo->no);
                }
                long start_time = getCurrentTime();
                if (video_decode(av_info->vd_handle, (char *)data + head_len, frameInfo->len, av_info->tempbuf, &av_info->width, &av_info->height, &Outsize) < 0)
                {
                    LOGE("1Outsize = %d", Outsize);
                    LOGE("video_decode");
                    return;
                }
                //  LOGE("time-consuming-old-1 = %d",(getCurrentTime()-start_time+begin_consuming));
                // int k = 1;
                // int h = 0;
                // for (int i = 0; i < 100; i++)
                // {
                //     for(int j = 0; j < VIDEO_BUF_LEN; j++){
                //         k += av_info->tempbuf[j];
                //         h +=1;
                //     }
                // }
                // LOGE("frame_num_start:time = %d h = %d ",(getCurrentTime()-start_time),h);
                // if (1==1)
                // {
                //     return;
                // }
                
               


                if (showTimeInfo)
                {
                    LOGE("jni_start end_decode sec = %d usec = %d no = %d", frameInfo->sec, frameInfo->usec, frameInfo->no);
                }
                // LOGE("jni_codec_end = %d",frameInfo->no);
                //LOGE("2Outsize = %d",Outsize);
                // LOGE("aaaahandle10 = %d len = %d tempbuf = %d",handle,len,av_info->tempbuf);
                ShowVideo(handle, (unsigned char *)av_info->tempbuf, Outsize, av_info->width, av_info->height, 0, frameInfo->no, 0);
                //   LOGE("aaaahandle11 = %d len = %d",handle,len);
            }
           
        }
    }

    if (frameInfo->frame_type == 3)
    {
        //LOGE("freame type == %d %d %d %d",*((unsigned char*)data+head_len),*((unsigned char*)data+head_len+1),*((unsigned char*)data+head_len+2),*((unsigned char*)data+head_len+3));

        /*if(tick>av_info->audio_tick)
        {
            av_info->audio_tick = tick;
            av_info->audio_len = 0;
        }
        if(av_info->audio_len> AUDIO_IN_SECOND)
        {
            return;
        }*/

        memset(av_info->audiobuf, 0, AUDIO_BUF_LEN);

        unsigned int outsize;
        //LOGE("subtype %d %d",frameInfo->frame_sub_type,frameInfo->len);

        outsize = ALawDecode_len((short *)av_info->audiobuf, (const uint8_t *)data + head_len, (size_t)frameInfo->len);

        //        LOGE("ShowAudio  : hard %d",handle);

        //av_info->audio_len += outsize;
        ShowAudio(handle, av_info->audiobuf, outsize, 1);
    }
    /*  gettimeofday(&tv_end, NULL);

    if(tv_start.tv_sec == tv_end.tv_sec){
        LOGE("jni_start time %d",tv_end.tv_usec-tv_start.tv_usec);
        if(tv_end.tv_usec-tv_start.tv_usec>30*1000)
            LOGE("jni_start time11 %d",tv_end.tv_usec-tv_start.tv_usec);
    }else{
        LOGE("jni_start time %d",tv_end.tv_usec+1000000-tv_start.tv_usec);
        if(tv_end.tv_usec+1000000-tv_start.tv_usec>30*1000)
         LOGE("jni_start time111 %d",tv_end.tv_usec+1000000-tv_start.tv_usec);
    }*/

    // if (frameInfo->frame_type != 3)
    // LOGE("jni_start sec = %d usec = %d no = %d",frameInfo->sec,frameInfo->usec,frameInfo->no);
}


long getCurrentTime()  
{  
   struct timeval tv;  
   gettimeofday(&tv,NULL);  
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;  
} 

void StreamThread(void *param)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" StreamThread 异常！");
        return;
    }
    exception_catch();

    pd_manager *manager = (pd_manager *)param;
    while (manager->pid_flag == 1)
    {
        struct pollfd pset;
        pset.fd = manager->sock;
        pset.events = POLLIN;

        int ret = poll(&pset, 1, 1000);
        if (ret < 0)
        {
            LOGE("poll CloseStream = %s", manager->handle);
            CloseStream(manager->handle);
            break;
        }
        if (ret == 0)
            continue;

        if (manager->used != 1)
            continue;
        //LOGE("read");
        if (manager->buffer)
        {
            ret = Recv(manager->sock, manager->buffer, sizeof(ptc_frame_head_t), 0);
            if (ret != sizeof(ptc_frame_head_t))
            {
                LOGE("ret != sizeof(ptc_frame_head_t) CloseStream = %d", manager->handle);
                CloseStream(manager->handle);
                break;
            }
            ptc_frame_head_t *frameInfo = (ptc_frame_head_t *)manager->buffer;
        
            // LOGE("frame_num_start:face:id= %d ",frameInfo->no);
            // LOGE("jni_start sec = %d usec = %d no = %d",frameInfo->sec,frameInfo->usec,frameInfo->no);

            ret = Recv(manager->sock, manager->buffer + sizeof(ptc_frame_head_t), frameInfo->len, 0);
            if (ret != frameInfo->len)
            {
                LOGE("Recv CloseStream = %s , ret = %d , len = %d ", manager->handle, ret, frameInfo->len);
                CloseStream(manager->handle);
                break;
            }

            int len = frameInfo->len + sizeof(ptc_frame_head_t);
            // LOGI("buf_size = %d ; buf_value = %s ; ",strlen(manager->buffer),manager->buffer);
            // long start_time = getCurrentTime();
            LiveDataCallback2(manager->handle, manager->buffer, len, manager->param);
            // if(frameInfo->frame_type == 1 || frameInfo->frame_type == 2)
            //     LOGE("frame_num_start:time = %d   %d  ",manager->handle,( getCurrentTime() - start_time));
        }
    }
    //  closeStream(manager->handle);
    /* CloseStream(manager->handle);
    LOGE("close+++++++%d",manager->sock);
    shutdown(manager->sock,2);
    close(manager->sock);
    if(manager->buffer)
        free(manager->buffer);
    manager->used = 0;*/
}

//socket通信
int OpenStream(int stream, void *param)
{
    if (sigsetjmp(JUMP_ANCHOR, 1) != 0)
    {
        LOGI(" OpenStream 异常！");
        return -1;
    }
    exception_catch();

    struct sockaddr_in servaddr;
    int sock = -1;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        LOGE("socket failed error[%d:%s]\n", errno, strerror(errno));
        return -1;
    }
    LOGE("setsockopt %d", sock);
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error \n");
        LOGE(" %s", "inet_pton error");
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    int flag = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(int)))
    {
        printf("setsockopt SO_REUSEADDR");
        LOGE(" %s", "setsockopt SO_REUSEADDR");
        shutdown(sock, 2);
        close(sock);
        return -1;
    }

    /* int keepAlive = 1; // 弿启keepalive属濿
     setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(int));
         if (bind(socket, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) < 0)
     {
         log_errorNo("bind(%d)", sock);
         return -1;
     }*/

    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        LOGE("connect error: %s(errno: %d)", strerror(errno), errno);
        shutdown(sock, 2);
        close(sock);
        return -1;
    }

    pd_msg_t msg;
    msg.cmd = 1;
    msg.ch = stream;
    int ret = write(sock, &msg, sizeof(pd_msg_t));
    LOGE("write ret = %d", ret);
    if (ret < 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    struct pollfd pset;
    pset.fd = sock;
    pset.events = POLLIN;

    ret = poll(&pset, 1, 1000);
    LOGE("poll ret = %d", ret);
    if (ret <= 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    pd_msg_t rmsg;
    rmsg.res = -1;
    read(sock, &rmsg, sizeof(pd_msg_t));
    LOGE("read rmsg.res = %d", rmsg.res);
    if (rmsg.res != 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    int id = getManagerUnused();
    LOGE("getManagerUnused = %d", id);
    if (id < 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    mPdManager[id].used = 1;
    mPdManager[id].pid_flag = 1;
    mPdManager[id].buffer = (char *)malloc(NET_VIDEO_BUF);
    mPdManager[id].param = param;
    if (pthread_create(&mPdManager[id].pid, NULL, StreamThread, (void *)&mPdManager[id]) != 0)
    {
        //       LOGE("pthread_create = %d",pthread_create(&mPdManager[id].pid,NULL,StreamThread,(void*)&mPdManager[id]));
        shutdown(sock, 2);
        close(sock);
        if (mPdManager[id].buffer)
            free(mPdManager[id].buffer);
        return -1;
    }
    mPdManager[id].sock = sock;
    mPdManager[id].stream = stream;

    mPdManager[id].handle = id;
    mPdManager[id].status = 0;
    return id;
}
