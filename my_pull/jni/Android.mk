LOCAL_PATH := $(call my-dir)

# include $(CLEAR_VARS)
# LOCAL_MODULE    := avcodec
# LOCAL_SRC_FILES := libavcodec.a
# include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS -Wno-sign-compare -Wno-switch -Wno-pointer-sign -DHAVE_NEON=1
      -mfpu=neon -mfloat-abi=softfp -fPIC -DANDROID -std=c++1y

LOCAL_C_INCLUDES += -$(LOCAL_PATH)/ \
					-$(LOCAL_PATH)/include/

LOCAL_SRC_FILES :=  Main.c PullStream.c FFDecode.c 
LOCAL_MODULE := decode

LOCAL_LDLIBS :=-L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib \
-L$(LOCAL_PATH) -lavformat -lavcodec -lavutil -lswscale \
-llog -lz -ldl -lgcc -fexceptions -fPIC -shared -lGLESv2 -lOpenSLES -landroid -lc

# LOCAL_LDLIBS :=-L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib \
# -L$(LOCAL_PATH)  -lavcodec \
# -llog -lz -ldl -lgcc -fexceptions -fPIC -shared -lGLESv2 -lOpenSLES -landroid -lc

# LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)