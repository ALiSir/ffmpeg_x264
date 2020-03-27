LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := avfilter
LOCAL_SRC_FILES := libavfilter.a  #这几个prebuild是为了把库预编译一下，ndk会将它移动到libs目录下面去
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
      -mfpu=neon -mfloat-abi=softfp -fPIC -DANDROID  #这里的Cflag是照抄之前的config.sh里面的，实际可能用不到这么多

LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/ffmpeg

LOCAL_SRC_FILES :=  G711.c playlib.c UvcNative.c

LOCAL_LDLIBS :=-L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib \
-L$(LOCAL_PATH)  -lavformat -lavcodec -lavdevice -lavfilter -lavutil -ljpeg\
-lswscale -lswresample -llog -lz -ldl -lgcc -fexceptions -fPIC -shared -lGLESv2 -lOpenSLES -landroid -lc
#这两个-L很重要， 保证程序能找到库
LOCAL_MODULE := UvcNative 
include $(BUILD_SHARED_LIBRARY)