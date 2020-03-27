#include <jni.h>


#ifndef _Included_com_sunbo_idarling_UvcNative
#define _Included_com_sunbo_idarling_UvcNative



#ifdef __cplusplus
extern "C"{
#endif

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Init(JNIEnv *env, jobject obj);
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Done(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetHardDecode(JNIEnv *env, jobject obj,int handle,int type);

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetPFrameNum(JNIEnv *env, jobject obj,int num);

JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_SetJastIframe(JNIEnv *env, jobject obj,int handle,int type);
/*
 * Class:     com_sunbo_idarling_UvcNative
 * Method:    Open
 * Signature: ()Z
 */
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Open(JNIEnv *, jobject,jint channel,int hardDecode,int justIframe);

/*
 * Class:     com_sunbo_idarling_UvcNative
 * Method:    Close
 * Signature: ()Z
 */
JNIEXPORT jint JNICALL Java_com_sanbot_opensdk_function_unit_MediaManager_Close(JNIEnv *, jobject,jint handle);

#ifdef __cplusplus
}
#endif
#endif
