LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src/main/java)
LOCAL_PACKAGE_NAME := hctestapp
LOCAL_SDK_VERSION := system_current
LOCAL_MIN_SDK_VERSION := 23
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/src/main/res
LOCAL_MANIFEST_FILE := src/main/AndroidManifest.xml
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_JNI_SHARED_LIBRARIES := libhardcoder
LOCAL_STATIC_JAVA_LIBRARIES:= libapp2sys

include $(BUILD_PACKAGE)
