#include "com_tencent_mm_hardcoder_HardCoderJNI.h"
#include <jni.h>
#include <map>
#include "client.h"
#include "procutil.h"

char* jstringToChars(JNIEnv *env, jstring jstr) {
    jboolean isCopy = JNI_FALSE;
	const char* str = env->GetStringUTFChars(jstr, &isCopy);
	char* ret = strdup(str);
	env->ReleaseStringUTFChars(jstr, str);
    return ret;
}

int* jintArrayToInts(JNIEnv *env, jintArray sarr, int &len) {
    if (sarr == NULL) {
        len = 0;
        return NULL;
    }

    jsize alen = env->GetArrayLength(sarr);
    jint *sa = env->GetIntArrayElements(sarr, JNI_FALSE);
    int *rtn = NULL;
    if (alen > 0) {
        rtn = new int[alen];
        memcpy(rtn, sa, alen * sizeof(int));
    }
    env->ReleaseIntArrayElements(sarr, sa, 0);
    len = alen;
    return rtn;
}

char* jbyteArrayToChars(JNIEnv *env, jbyteArray barr, int &len) {
    if (barr == NULL) {
        len = 0;
        return NULL;
    }

    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    char* rtn = NULL;
    if (alen > 0) {
        rtn = new char[alen];
        memcpy(rtn, ba, alen);
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    len = alen;
    return rtn;
}


static Client instance;


class Callback : public IC2JavaCallback {
private:
    int m_init;

public:
    Callback() {
        m_init = 0;
    }

    void init() {
        if (m_init) {
            return;
        }

        m_init = 1;

        pthread_t tid;
        pthread_create(&tid, NULL, callbackThread, this);
    }

    class CallbackInfo {
    public:
        int callbackType;
        uint64_t requestid;
        uint64_t timestamp;
        int errCode;
        int funcid;
        int dataType;
        uint8_t *data;
        int len;

        ~CallbackInfo() {
            if (data) {
                delete[] data;
                data = NULL;
                len = 0;
            }
        }

        CallbackInfo(int callbackType, uint64_t requestid, uint64_t timestamp, int errCode, int funcid, int dataType, uint8_t *data, int len) {
            this->callbackType = callbackType;
            this->requestid = requestid;
            this->timestamp = timestamp;
            this->errCode = errCode;
            this->funcid = funcid;
            this->dataType = dataType;
            this->data = NULL;
            this->len = 0;
            if (data != NULL && len > 0) {
                this->data = new uint8_t[len];
                memcpy(this->data, data, len);
                this->len = len;
            }
        }
    };

    rqueue<CallbackInfo *, 0> queue;

    static void* callbackThread(void *args) {
        JNIEnv *env = NULL;
        int ret = g_jvm->AttachCurrentThread(&env, NULL);
        if (ret != JNI_OK) {
            perr("callbackThread, AttachCurrentThread failed, jvm:%d, ret:%d", TOINT(g_jvm), ret);
            return NULL;
        }

        Callback *thiz = (Callback *) args;
        while (1) {
            CallbackInfo *info = thiz->queue.pop();
            if (instance.isEnable()) {
                c2Java_OnData(env, info->callbackType, info->requestid, info->timestamp, info->errCode, info->funcid,
                              info->dataType, info->data, info->len);
            }
            delete info;
        }
        return NULL;
    }

    int callback(int callbackType, uint64_t requestid, uint64_t timestamp, int errCode, int funcid, int type, uint8_t *data, int len) {
        CallbackInfo *info = new CallbackInfo(callbackType, requestid, timestamp, errCode, funcid, type, data, len);
        queue.push(info);
        return 0;
    }
};

static Callback callback;


JNIEXPORT jint JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_initHardCoder(JNIEnv *env, jclass jcls,
        jstring remote, jint port, jstring local) {
    UNUSED(jcls);
    pdbg("initHardCoder, start.");
    char *r = jstringToChars(env, remote);
    char *l = jstringToChars(env, local);
    callback.init();
    instance.init(r, port, l, &callback);
    pdbg("initHardCoder, end.");
    free(r);
    free(l);
    return 0;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_checkPermission(JNIEnv *env, jclass jcls,
        jobjectArray manufactures, jobjectArray certs, jint pid, jint uid, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);

    jsize manufacturesCount = (*env).GetArrayLength(manufactures);
    if (manufacturesCount <= 0) {
        perr("checkPermission: manufactures count is zero!");
    }
    pdbg("checkPermission: manufactures count is %d", manufacturesCount);
    std::vector<std::string> manufacturesList;
    for (jsize index = 0; index < manufacturesCount; index++) {
        char *c = jstringToChars(env, (jstring)(*env).GetObjectArrayElement(manufactures, index));
        std::string manufacture = (c == NULL ? "" : c);
        free(c);
        manufacturesList.push_back(manufacture);
        pdbg("checkPermission: index[%d] manufacture[%s]", index, manufacture.c_str());
    }

    jsize certsCount = (*env).GetArrayLength(certs);
    if (certsCount <= 0) {
        perr("checkPermission: certs count is zero!");
    }
    pdbg("checkPermission: certs count is %d", certsCount);
    std::vector<std::string> certsList;
    for (jsize index = 0; index < certsCount; index++) {
        char *c = jstringToChars(env, (jstring)(*env).GetObjectArrayElement(certs, index));
        std::string manufacture = (c == NULL ? "" : c);
        free(c);
        certsList.push_back(manufacture);
        pdbg("checkPermission: index[%d] manufacture[%s]", index, manufacture.c_str());
    }

    int64_t requestId = instance.checkPermission(manufacturesList, certsList, pid, uid, tid, timestamp);
    pdbg("checkPermission, requestId:%lld, manufactures:%d, certs:%d, pid:%d, uid:%d, tid:%d, timestamp:%lld ",
            manufacturesList.size(), certsList.size(), requestId, pid, uid, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestCpuHighFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeoutms, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.requestCpuHighFreq(scene, action, level, timeoutms, tid, timestamp);
    pdbg("requestCpuHighFreq, requestId:%lld, scene:%d, action:%d, level:%d, timeoutms:%d, tid:%d, timestamp:%" PRId64,
            requestId, scene, TOINT(action), level, timeoutms, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelCpuHighFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.cancelCpuHighFreq(tid, timestamp);
    pdbg("cancelCpuHighFreq, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestGpuHighFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeoutms, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.requestGpuHighFreq(scene, action, level, timeoutms, tid, timestamp);
    pdbg("requestGpuHighFreq, requestId:%lld, scene:%d, action:%d, level:%d, timeoutms:%d, tid:%d, timestamp:%" PRId64,
            requestId, scene, TOINT(action), level, timeoutms, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelGpuHighFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.cancelGpuHighFreq(tid, timestamp);
    pdbg("cancelGpuHighFreq, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestCpuCoreForThread(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jintArray bindtids, jint timeoutms, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    int* pbindtids = jintArrayToInts(env, bindtids, len);
    int64_t requestId = instance.requestCpuCoreForThread(scene, action, pbindtids, len, timeoutms, tid, timestamp);
    pdbg("requestCpuCoreForThread, requestId:%lld, scene:%d, action:%d, len:%d, timeoutms:%d, tid:%d, timestamp:%" PRId64,
            requestId, scene, TOINT(action), len, timeoutms, tid, timestamp);
    delete[] pbindtids;
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelCpuCoreForThread(JNIEnv *env, jclass jcls,
        jintArray bindtids, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    int *pbindtids  = jintArrayToInts(env, bindtids, len);
    pdbg("cancelCpuCoreForThread: len:%d, tid:%d, timestamp:%" PRId64, len, tid, timestamp);
    int64_t requestId = instance.cancelCpuCoreForThread(pbindtids, len, tid, timestamp);
    delete[] pbindtids;
    return requestId;
}
JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestHighIOFreq(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint level, jint timeoutms, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.requestHighIOFreq(scene, action, level, timeoutms, tid, timestamp);
    pdbg("requestHighIOFreq, requestId:%lld, scene:%d, action:%d, level:%d, timeoutms:%d, tid:%d, timestamp:%" PRId64,
            requestId, scene, TOINT(action), level, timeoutms, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelHighIOFreq(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.cancelHighIOFreq(tid, timestamp);
    pdbg("cancelHighIOFreq, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestScreenResolution(JNIEnv *env, jclass jcls,
        jint level, jstring uiName, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    char *uiNamestr = jstringToChars(env, uiName);
    int64_t requestId = instance.requestScreenResolution(level, uiNamestr == NULL ? "" : uiNamestr, tid, timestamp);
    pdbg("requestScreenResolution, requestId:%lld, level:%d, uiName:%s, tid:%d, timestamp:%" PRId64,
            requestId, level, uiNamestr == NULL ? "" : uiNamestr, tid, timestamp);
    free(uiNamestr);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_resetScreenResolution(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.resetScreenResolution(tid, timestamp);
    pdbg("resetScreenResolution, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerANRCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.registerANRCallback(tid, timestamp);
    pdbg("registerANRCallback, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerSystemEventCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.registerSystemEventCallback(tid, timestamp);
    pdbg("registerSystemEventCallback, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerBootPreloadResource(JNIEnv *env, jclass jcls,
        jobjectArray files, jint tid, jlong timestamp) {
    UNUSED(jcls);
    pdbg("registerBootPreloadResource: %d %" PRId64, tid, timestamp);
    jsize filesCount = (*env).GetArrayLength(files);
    if (filesCount <= 0) {
        perr("registerBootPreloadResource: files count is zero!");
    }
    pdbg("registerBootPreloadResource: files count is %d", filesCount);
    std::vector<std::string> fileList;
    for (jsize index = 0; index < filesCount; index++) {
        char *c = jstringToChars(env, (jstring)(*env).GetObjectArrayElement(files, index));
        std::string file = (c == NULL ? "" : c);
        free(c);
        fileList.push_back(file);
        pdbg("registerBootPreloadResource: index[%d] file[%s]", index, file.c_str());
    }
    return instance.registerBootPreloadResource(fileList, tid, timestamp);
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_terminateApp(JNIEnv *env, jclass jcls, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.terminateApp(tid, timestamp);
    pdbg("terminateApp, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_requestUnifyCpuIOThreadCoreGpu(JNIEnv *env, jclass jcls,
        jint scene, jlong action, jint cpulevel, jint gpulevel, jint iolevel, jintArray bindtids, jint timeout, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    int *pbindtids = jintArrayToInts(env, bindtids, len);
    int64_t requestId = instance.requestUnifyCpuIOThreadCoreGpu(scene, action, cpulevel, iolevel, pbindtids, len, gpulevel, timeout, tid, timestamp);
    pdbg("requestUnifyCpuIOThreadCoreGpu, requestId:%lld, scene:%d, action:%d, cpulevel:%d, iolevel:%d, bindtids len:%d, "
         "gpulevel:%d timeout:%d tid:%d timestamp:%" PRId64, requestId, scene, TOINT(action), cpulevel, iolevel, len, gpulevel, timeout, tid, timestamp);
    delete[] pbindtids;
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_cancelUnifyCpuIOThreadCoreGpu(JNIEnv *env, jclass jcls,
        jboolean cancelcpu, jboolean cancelgpu, jboolean cancelio, jboolean cancelthread, jintArray bindtids, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    int *pbindtids  = jintArrayToInts(env, bindtids, len);
    int64_t requestId = instance.cancelUnifyCpuIOThreadCoreGpu(cancelcpu ? 1 : 0, cancelio ? 1 : 0, cancelthread ? 1 : 0, pbindtids, len, cancelgpu ? 1 : 0, tid, timestamp);
    pdbg("cancelUnifyCpuIOThreadCoreGpu, requestId:%lld, cancelcpu:%d, cancelio:%d, cancelthread:%d, bindtids len:%d, "
         "cancelgpu:%d, tid:%d, timestamp:%" PRId64, requestId, cancelcpu, cancelio, cancelthread, len, cancelgpu, tid, timestamp);
    delete[] pbindtids;
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_configure(JNIEnv *env, jclass jcls,
        jbyteArray data, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    char* dataBuf = jbyteArrayToChars(env, data, len);
    pdbg("configure, data len:%d %d %" PRId64, len, tid, timestamp);
    return instance.configure(dataBuf == NULL ? "" : dataBuf, tid, timestamp);
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getParameters(JNIEnv *env, jclass jcls,
        jbyteArray data, jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int len;
    char* dataBuf = jbyteArrayToChars(env, data, len);
    pdbg("getParameters, data len:%d %d %" PRId64, len, tid, timestamp);
    return instance.getParameters(dataBuf == NULL ? "" : dataBuf, tid, timestamp);
}

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setHCEnable(JNIEnv *env, jclass jcls,
        jboolean enable) {
    UNUSED(env);
    UNUSED(jcls);
    pdbg("setEnable: %d", enable ? 1 : 0);
    instance.setEnable(enable ? 1 : 0);
}

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setRetryConnectInterval(JNIEnv *env, jclass jcls,
        jint interval) {
    UNUSED(env);
    UNUSED(jcls);
    pdbg("setRetryConnectInterval: %d", interval);
    instance.setRetryConnectInterval(interval);
}

JNIEXPORT void JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_setDebug(JNIEnv *env, jclass jcls,
        jboolean debug) {
    DEBUG = debug;
    pdbg("setDebug: %d", debug ? 1 : 0);
}

JNIEXPORT jint JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_isRunning(JNIEnv *env, jclass jcls) {
    UNUSED(env);
    UNUSED(jcls);
    return instance.isRunning() ? 1 : 0;
}

static std::map<int, ThreadJiffiesMonitor*> g_monitorMap;

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_startTraceCpuLoad(JNIEnv *env, jclass jcls,
        jint linuxTid, jint sampleRate) {
    UNUSED(env);
    UNUSED(jcls);
    ThreadJiffiesMonitor *pmonitor = new ThreadJiffiesMonitor;
    if (pmonitor->start(linuxTid, sampleRate) < 0) {
        return 0;
    }
    g_monitorMap[TOINT(pmonitor)] = pmonitor;
    return TOINT(pmonitor);
}

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_stopTraceCpuLoad(JNIEnv *env, jclass jcls, jint id) {
    UNUSED(env);
    UNUSED(jcls);
    if (!id) {
        return -1;
    }
    ThreadJiffiesMonitor *pmonitor = g_monitorMap[id];
    uint64_t unixTime, cpuLoad;
    if (pmonitor->stop(unixTime, cpuLoad) < 0) {
        return 0;
    }
    return (int) cpuLoad;
}

JNIEXPORT int JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getTickRate(JNIEnv *env, jclass jcls) {
    UNUSED(env);
    UNUSED(jcls);
    return (int) sysconf(_SC_CLK_TCK);
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerCpuLoadCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.registerCpuLoadCallback(tid, timestamp);
    pdbg("registerCpuLoadCallback, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_registerDeviceTemperatureCallback(JNIEnv *env, jclass jcls,
        jint tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.registerDeviceTemperatureCallback(tid, timestamp);
    pdbg("registerDeviceTemperatureCallback, requestId:%lld, tid:%d, timestamp:%" PRId64, requestId, tid, timestamp);
    return requestId;
}

JNIEXPORT jlong JNICALL Java_com_tencent_mm_hardcoder_HardCoderJNI_getCurrentTemperaturesWithType(JNIEnv *env, jclass jcls,
        jint type, int tid, jlong timestamp) {
    UNUSED(env);
    UNUSED(jcls);
    int64_t requestId = instance.getCurrentTemperaturesWithType(type, tid, timestamp);
    pdbg("getCurrentTemperaturesWithType, requestId:%lld, type:%d, tid:%d, timestamp:%" PRId64,
         requestId, type, tid, timestamp);
    return requestId;
}
