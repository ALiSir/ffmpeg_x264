#include "PullStream.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <android/log.h>
#include <poll.h>
#define LOG_TAG "sanbot_decode"
#define Lg(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)

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
} sc_mng;

typedef struct
{
    int cmd; //1:open video,2:close video,3:heartbeat
    int ch;  //0:mian stream,1:sub stream
    int res; //0:suc,1:failed
} pd_msg_t;

#define AVCODEC_NUM 2

av_t av[AVCODEC_NUM];

#define SERVER_PORT 5500
#define SC_MNG_NUM 2
#define NET_VIDEO_BUF 800 * 1024

sc_mng scMng[SC_MNG_NUM];

char *streamBack;

void StreamInit()
{
    memset(&av, 0, AVCODEC_NUM * sizeof(av_t));
    for (int i = 0; i < AVCODEC_NUM; i++)
    {
        av[i].handle = -1;
    }
}

int recvBuf(int fd, char *buf, int len)
{

    char *tmpBuf = buf;
    int residualLen = len;

    while (residualLen > 0)
    {
        int ret = recv(fd, tmpBuf, residualLen, 0);
        if (ret < 0)
        {
            return -1;
        }
        if (ret == 0)
        {
            break;
        }
        tmpBuf += ret;
        residualLen -= ret;
    }
    return len - residualLen;
}

void PullThread(void *param)
{
    sc_mng *scTmp = (sc_mng *)param;
    while (scTmp->pid_flag == 1)
    {
        struct pollfd pset;
        pset.fd = scTmp->sock;
        pset.events = POLLIN;
        int ret = poll(&pset, 1, 1000);
        if (ret < 0)
        {
            Lg("pull stream faild , close Stream = %d", scTmp->handle);
            closeStream(scTmp->handle);
            break;
        }
        if (ret == 0)
        {
            continue;
        }
        if (scMng->used == 0)
        {
            continue;
        }
        if (!scMng->buffer)
        {
            continue;
        }

        ret = recvBuf(scTmp->sock, scTmp->buffer, sizeof(ptc_frame_head_t));
        if (ret != sizeof(ptc_frame_head_t))
        {
            Lg("recv stream head failed,close stream %d", scTmp->handle);
            closeStream(scTmp->handle);
            break;
        }

        ptc_frame_head_t *frameInfo = (ptc_frame_head_t *)scTmp->buffer;

        ret = recvBuf(scTmp->sock, scTmp->buffer + sizeof(ptc_frame_head_t), frameInfo->len);
        if (ret != frameInfo->len)
        {
            Lg("recv stream content failed,close stream %d , need length = %d ,but reality length = %d", scTmp->handle,frameInfo,ret);
            closeStream(scTmp->handle);
            break;
        }

        int len = frameInfo->len + sizeof(ptc_frame_head_t);
        streamCallBack(scTmp->handle, scTmp->buffer, len, scTmp->param);
        // Lg("recv data, data size = %d", len);
    }
}

int sendPullMsg(int ch, void *param)
{
    int sock = -1;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        Lg("socket failed error[%d:%s]\n", errno, strerror(errno));
        return -1;
    }
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        Lg("inet_pton error");
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    int flag = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(int)))
    {
        Lg("setsockopt SO_REUSEADDR");
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        Lg("connect error: %s(errno: %d)", strerror(errno), errno);
        shutdown(sock, 2);
        close(sock);
        return -1;
    }

    pd_msg_t msg;
    msg.cmd = 1;
    msg.ch = ch;
    int ret = write(sock, &msg, sizeof(pd_msg_t));
    Lg("send cmd = 1,ch = %d,ret = %d", ch, ret);
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
    Lg("poll ret = %d", ret);
    if (ret <= 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    pd_msg_t rmsg;
    rmsg.res = -1;
    read(sock, &rmsg, sizeof(pd_msg_t));
    Lg("read rmsg.res = %d", rmsg.res);
    if (rmsg.res != 0)
    {
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    int id = 0;
    for (; id < SC_MNG_NUM; id++)
    {
        if (scMng[id].used == 0)
        {
            break;
        }
    }
    if (id >= SC_MNG_NUM)
    {
        Lg("pull stream failed,reason : sc mng num than max ,is %d", id);
        shutdown(sock, 2);
        close(sock);
        return -1;
    }
    sc_mng *scTmp = &scMng[id];
    scTmp->used = 1;
    scTmp->pid_flag = 1;
    scTmp->buffer = (char *)malloc(NET_VIDEO_BUF);
    scTmp->param = param;

    if (pthread_create(&(scTmp->pid), NULL, PullThread, (void *)scTmp) != 0)
    {
        Lg("create pull stream thread failed,handle = %d", id);
        shutdown(sock, 2);
        close(sock);
        if (scTmp->buffer)
            free(scTmp->buffer);
        return -1;
    }

    scTmp->sock = sock;
    scTmp->stream = ch;
    scTmp->handle = id;
    scTmp->status = 0;
    Lg("open socket stream suc , handle = %d", id);
    Lg("index = %d ; scTmp.handle = %d ; scTmp.ch = %d ; scTmp.sock = %d", id, scTmp->handle, scTmp->stream, scTmp->sock);
    return id;
}

int openStream(int ch, int hardDecode, int justIframe, *streamCallBack(int handle, unsigned char *buf, int len, void *pUser))
{
    Lg("open ch = %d,handDecode = %d,justIFrame = %d", ch, hardDecode, justIframe);

    streamBack = streamCallBack;

    av_t *av_info;
    int i = 0;
    for (; i < AVCODEC_NUM; i++)
    {
        if (av[i].handle < 0)
        {
            break;
        }
    }
    if (i >= AVCODEC_NUM)
    {
        Lg(" open stream failed,pull stream max than AVCODEC_NUM,max i = %d", i);
        return -1;
    }

    av_info = &av[i];
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
    av_info->vd_handle = 0;

    int handle = sendPullMsg(ch, av_info);
    if (handle < 0)
    {
        Lg("open sream failed,handle  = %d", handle);
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
        return -1;
    }

    av_info->handle = handle;
    av_info->hardDecode = hardDecode;
    av_info->justIframe = justIframe;

    Lg("open stream suc,handle = %d", handle);
    return handle;
}

int closeStream(int streamId)
{
    Lg("start close stream id = %d",streamId);
    av_t *avc = NULL;
    sc_mng *scmngc = NULL;
    for (int i = 0; i < sizeof(av) / sizeof(av[0]); i++)
    {
        if (av[i].handle == streamId)
        {
            avc = &av[i];
            break;
        }
    }
    for (int i = 0; i < sizeof(scMng) / sizeof(scMng[0]); i++)
    {
        if (scMng[i].handle == streamId)
        {
            Lg("close sc_mng index = %d", i);
            scmngc = &scMng[i];
            break;
        }
    }
    if (avc == NULL || scmngc == NULL)
    {
        Lg("avc == null || scmngc == null");
        return -1;
    }
    pd_msg_t msg;
    msg.cmd = 2;
    msg.ch = scmngc->stream;
    int ret = write(scmngc->sock, &msg, sizeof(pd_msg_t));
    Lg(" close stream of streamId = %d , ch = %d , ret = %d", scmngc->handle, msg.ch, ret);

    close(scmngc->sock);
    shutdown(scmngc->sock, 2);

    if (scmngc->buffer != NULL)
    {
        free(scmngc->buffer);
        scmngc->buffer = NULL;
    }

    scmngc->status = -1;
    scmngc->used = 0;
    avc->handle = -1;
    Lg("close stream suc streamId = %d",streamId);
    return 1;
}
