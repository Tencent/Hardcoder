LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDE := $(LOCAL_PATH)/src/main/cpp/cjson
LOCAL_SRC_FILES := src/main/cpp/cjson/cJSON.c
LOCAL_CFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -DANDROID_STL=c++_shared
LOCAL_CXXFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -DANDROID_STL=c++_shared
LOCAL_MODULE := libcjson
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/src/main/cpp/cjson
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libamc_proto
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_CFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared
LOCAL_CXXFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared
LOCAL_SRC_FILES := $(call all-proto-files-under, src/main/proto)
generated_sources_dir := $(call local-generated-sources-dir)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(generated_sources_dir)/proto/$(LOCAL_PATH)/src/main/proto
#LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/prebuilt/include/google/protobuf
#LOCAL_C_INCLUDE := $(LOCAL_PATH)/prebuilt/include/google/protobuf
LOCAL_SHARED_LIBRARIES := libprotobuf-cpp-lite
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PROTOC_OPTIMIZE_TYPE := lite
LOCAL_CFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared
LOCAL_CXXFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared
LOCAL_SRC_FILES := src/main/cpp/server.cpp
#    src/main/proto/amc.proto

LOCAL_MODULE := server
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDE := $(LOCAL_PATH)/src/main/cpp

LOCAL_MODULE_CLASS := EXECUTABLES

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-lite

LOCAL_STATIC_LIBRARIES := libcjson
LOCAL_STATIC_LIBRARIES += libamc_proto

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared -Wno-format
LOCAL_CXXFLAGS := -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden -Wno-unused-parameter -DANDROID_STL=c++_shared -Wno-format
LOCAL_C_INCLUDE :=$(LOCAL_PATH)/src/main/cpp
LOCAL_STATIC_LIBRARIES := libcjson
LOCAL_STATIC_LIBRARIES += libamc_proto
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-lite
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_MODULE := libhardcoder
LOCAL_SRC_FILES := src/main/cpp/com_tencent_mm_hardcoder_HardCoderJNI.cpp

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src/main/java) $(call all-proto-files-under, src/main/proto)
LOCAL_MODULE := libapp2sys
LOCAL_SDK_VERSION := system_current
LOCAL_MIN_SDK_VERSION := 23

#LOCAL_JNI_SHARED_LIBRARIES := libhardcoder
LOCAL_PROTOC_OPTIMIZE_TYPE := lite
LOCAL_PROTOC_FLAGS := --proto_path=$(LOCAL_PATH)/src/main/proto
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_STATIC_JAVA_LIBRARY)


