#include <jni.h>

#ifdef __cplusplus
extern "C"{
#endif

JNIEXPORT void JNICALL Java_com_sanbot_decode_Decode_init(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL Java_com_sanbot_decode_Decode_open(JNIEnv *env, jobject obj,jint ch,jint hardDecode,jint justIframe);

Java_com_sanbot_decode_Decode_close(JNIEnv *env, jobject obj,int streamId);

#ifdef __cplusplus
}
#endif