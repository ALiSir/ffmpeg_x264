LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := avfilter
LOCAL_SRC_FILES := libavfilter.a  #�⼸��prebuild��Ϊ�˰ѿ�Ԥ����һ�£�ndk�Ὣ���ƶ���libsĿ¼����ȥ
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := avutil
LOCAL_SRC_FILES := libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := avcodec
LOCAL_SRC_FILES := libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := avdevice
LOCAL_SRC_FILES := libavdevice.a
include $(PREBUILT_STATIC_LIBRARY)
 
include $(CLEAR_VARS)
LOCAL_MODULE    := swscale
LOCAL_SRC_FILES := libswscale.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := swresample
LOCAL_SRC_FILES := libswresample.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jpeg
LOCAL_SRC_FILES := libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)
  
include $(CLEAR_VARS)
LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS -Wno-sign-compare -Wno-switch -Wno-pointer-sign -DHAVE_NEON=1
      -mfpu=neon -mfloat-abi=softfp -fPIC -DANDROID  #�����Cflag���ճ�֮ǰ��config.sh����ģ�ʵ�ʿ����ò�����ô��

LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/ffmpeg

LOCAL_SRC_FILES :=  G711.c playlib.c UvcNative.c

LOCAL_LDLIBS :=-L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib \
-L$(LOCAL_PATH)  -lavformat -lavcodec -lavdevice -lavfilter -lavutil -ljpeg\
-lswscale -lswresample -llog -lz -ldl -lgcc -fexceptions -fPIC -shared -lGLESv2 -lOpenSLES -landroid -lc
#������-L����Ҫ�� ��֤�������ҵ���
LOCAL_MODULE := UvcNative 
include $(BUILD_SHARED_LIBRARY)