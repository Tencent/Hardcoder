cmake_minimum_required(VERSION 3.4.1)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unused -ffunction-sections -fdata-sections -fvisibility=hidden")

add_library(cJSON STATIC
    src/main/cpp/cjson/cJSON.c
)
target_include_directories(cJSON
    INTERFACE ${PROJECT_SOURCE_DIR}/src/main/cpp/cjson
)

add_library(amc_proto STATIC
    build/generated/source/proto/cpp/amc.pb.cc
)
target_include_directories(amc_proto
    INTERFACE ${PROJECT_SOURCE_DIR}/build/generated/source/proto/cpp
    PUBLIC ${PROJECT_SOURCE_DIR}/prebuilt/include
)
target_link_libraries(amc_proto
    PUBLIC ${PROJECT_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libprotobuf-lite.a
)

add_library(hardcoder SHARED
    src/main/cpp/com_tencent_mm_hardcoder_HardCoderJNI.cpp
)
target_link_libraries(hardcoder cJSON amc_proto log)
target_compile_definitions(hardcoder PRIVATE ANDROID_NDK DISABLE_IMPORTGL)

add_executable(server
    src/main/cpp/server.cpp
)
target_link_libraries(server cJSON amc_proto log)
target_compile_definitions(server PRIVATE ANDROID_NDK DISABLE_IMPORTGL)

#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/main/cpp)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/protobuf)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/protobuf/google)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/build/gen/cpp)

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib_output) // 如果单独编译so请开启这个设置，开启后test项目会没有so


#"arm64-v8a"
#add_executable(server
#             build/gen/cpp/amc.pb.cc
#             src/main/cpp/server.cpp
#             src/main/cjson/cJSON.c)
#
#if(${ANDROID_ABI} STREQUAL  "armeabi-v7a" OR ${ANDROID_ABI} STREQUAL "armeabi")
#    add_library(hardcoder SHARED
#                 build/gen/cpp/amc.pb.cc
#                 src/main/cpp/com_tencent_mm_hardcoder_HardCoderJNI.cpp
#                 src/main/cjson/cJSON.c
#    )
#
#
#    add_custom_command(TARGET hardcoder POST_BUILD
#                 COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/build/intermediates/cmake/debug/obj/${ANDROID_ABI}/libhardcoder.so
#                         ${PROJECT_SOURCE_DIR}/lib_output/${ANDROID_ABI}/libhardcoder.so)
#
#endif()

#ADD_DEFINITIONS(
#    -Wall -Werror -fPIE -DANDROID_NDK -DDISABLE_IMPORTGL -fvisibility=hidden -fPIC -fno-exceptions -Wno-multichar -fno-strict-aliasing -fstack-protector-strong -ffunction-sections -fdata-sections -funwind-tables -Wa,--noexecstack -Werror=format-security  -fno-short-enums -no-canonical-prefixes -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast -Werror=implicit-function-declaration -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Werror=date-time  -Wstrict-aliasing=2  -Werror=int-conversion -Wno-reserved-id-macro -Wno-format-pedantic -Wno-unused-command-line-argument -fcolor-diagnostics  -fvisibility-inlines-hidden  -Wsign-promo  -Wno-inconsistent-missing-override   -Werror=int-to-pointer-cast -Werror=pointer-to-int-cast  -Werror=address-of-temporary -Werror=null-dereference -Werror=return-type
#
#)
