# Build ImageProcessing

LOCAL_PATH := $(call my-dir)

# Build ImageProcessing library

include $(CLEAR_VARS)
 
OPENCV_LIB_TYPE:=STATIC
OPENCV_INSTALL_MODULES:=on

include /home/cobalt/Android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := ImageProcessing
LOCAL_SRC_FILES := ImageProcessing.cpp
LOCAL_LDLIBS +=  -llog -ldl -ljnigraphics

include $(BUILD_SHARED_LIBRARY)


# Build assetbridge library

include $(CLEAR_VARS)

LOCAL_MODULE := assetbridge
LOCAL_SRC_FILES := assetbridge.c

include $(BUILD_SHARED_LIBRARY)
