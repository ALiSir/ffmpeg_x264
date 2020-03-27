
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# APP_STL := stlport_static 

# APP_ABI := armeabi armeabi-v7a
APP_ABI := armeabi-v7a
APP_CPPFLAGS += -frtti
#APP_PLATFORM := android-23
# APP_STL := gnustl_shared
APP_STL := c++_static
