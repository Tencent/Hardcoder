/**
 * C to Java interface
 */

#ifndef HARDCODER_C2JAVA_H
#define HARDCODER_C2JAVA_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JavaVM *g_jvm = NULL;
jclass g_classUdpEngine;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    UNUSED(reserved);

    JNIEnv *env = NULL;
    jint result = JNI_ERR;
    g_jvm = vm;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jclass clazz = env->FindClass("com/tencent/mm/hardcoder/HardCoderJNI");
    g_classUdpEngine = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    return JNI_VERSION_1_6;
}

void c2Java_OnData(JNIEnv *envCThread, int callbackType, uint64_t requestid, uint64_t timestamp, int errCode, int funcid, int dataType, uint8_t *data, int len) {
    if (!envCThread) {
        perr("Not init env");
        return;
    }

    jmethodID methodId = envCThread->GetStaticMethodID(g_classUdpEngine, "onData", "(IJJIII[B)V");

    if (methodId == NULL) {
        perr("methodId is null");
        return;
    }

    jbyteArray jBuff = envCThread->NewByteArray(len);
    envCThread->SetByteArrayRegion(jBuff, 0, len, (jbyte *) data);

    envCThread->CallStaticVoidMethod(g_classUdpEngine, methodId, callbackType, (jlong)requestid, (jlong)timestamp, errCode,
                                     funcid, dataType, jBuff);

    envCThread->DeleteLocalRef(jBuff);
}


#ifdef __cplusplus
}
#endif

#endif //HARDCODER_C2JAVA_H
