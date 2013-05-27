LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include C:\softwear\Study\OpenCV-2.4.5-android-sdk\OpenCV-2.4.5-android-sdk\sdk\native\jni\OpenCV.mk


LOCAL_MODULE    := myjni
LOCAL_SRC_FILES := native.cpp

include $(BUILD_SHARED_LIBRARY)