#ifndef _Included_com_tencent_mm_hardcoder_HardCoderJNI
#define _Included_com_tencent_mm_hardcoder_HardCoderJNI

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************
 * JNI method
 *********************************************/

JNIEXPORT jint JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_initHardCoder(JNIEnv *env, jclass jcls,
        jstring remote, jint port, jstring local);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_checkPermission(JNIEnv *env, jclass jcls,
        jobjectArray manufactures, jobjectArray certs, jint pid, jint uid, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestCpuHighFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeout, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelCpuHighFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestGpuHighFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeout, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelGpuHighFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestCpuCoreForThread(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jintArray bindtids, jint timeoutms, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelCpuCoreForThread(JNIEnv *env, jclass jcls,
        jintArray bindtids, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestHighIOFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeoutms, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelHighIOFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestScreenResolution(JNIEnv *env, jclass jcls,
        jint level, jstring uiname, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_resetScreenResolution(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerANRCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerSystemEventCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerBootPreloadResource(JNIEnv *env, jclass jcls,
        jobjectArray files, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_terminateApp(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestUnifyCpuIOThreadCoreGpu(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint cpulevel, jint gpulevel, jint iolevel, jintArray bindtids, jint timeout, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelUnifyCpuIOThreadCoreGpu(JNIEnv *env, jclass jcls,
        jboolean cancelcpu, jboolean cancelgpu, jboolean cancelio, jboolean cancelthread, jintArray bindtids, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_configure(JNIEnv *env, jclass jcls,
        jbyteArray data, jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getParameters(JNIEnv *env, jclass jcls,
        jbyteArray data, jint tid, jlong timestamp);

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setHCEnable(JNIEnv *env, jclass jcls, jboolean enable);

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setRetryConnectInterval(JNIEnv *env, jclass jcls, jint interval);

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setDebug(JNIEnv *env, jclass jcls, jboolean debug);

JNIEXPORT jint JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_isRunning(JNIEnv *env, jclass jcls);

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_startTraceCpuLoad(JNIEnv *env, jclass jcls,
        jint linuxTid, jint sampleRate);

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_stopTraceCpuLoad(JNIEnv *env, jclass jcls, jint id);

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getTickRate(JNIEnv *env, jclass jcls);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerCpuLoadCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerDeviceTemperatureCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp);

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getCurrentTemperaturesWithType(JNIEnv *env, jclass jcls,
        jint type, jint tid, jlong timestamp);

#ifdef __cplusplus
}
#endif
#endif