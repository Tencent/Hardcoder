/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.  All rights reserved.
 *
 * Licensed under the BSD 3-Clause License.
 *
 * Terms of the BSD 3-Clause License:
 * --------------------------------------------------------------------
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.tencent.mm.hardcoder;

import android.content.Context;
import android.os.Process;
import android.os.SystemClock;

import java.lang.reflect.Method;
import java.util.HashMap;

public class HardCoderJNI {

    private final static String TAG = "Hardcoder.HardCoderJNI";

    static {
        System.loadLibrary("hardcoder");
    }

    private HardCoderJNI() {
    }

    /**
     * The cpu/io/gpu level that client request for.
     * level 0 means not change.
     * level 1 means request the highest level.
     * level 2 means the middle level.
     * level 3 means the lowest level, but also higher than the usual level
     */
    public final static int CPU_LEVEL_0 = 0;//not change
    public final static int CPU_LEVEL_1 = 1; // highest
    public final static int CPU_LEVEL_2 = 2;
    public final static int CPU_LEVEL_3 = 3;

    public final static int IO_LEVEL_0 = 0;//not change
    public final static int IO_LEVEL_1 = 1; // highest
    public final static int IO_LEVEL_2 = 2;
    public final static int IO_LEVEL_3 = 3;

    public final static int GPU_LEVEL_0 = 0;//not change
    public final static int GPU_LEVEL_1 = 1; // highest


    /**
     * funcid(function id) definition.
     * if funcid is zero, means this function id only use at client
     */
    public final static int FUNC_LOCAL = 0;

    public final static int FUNC_BASE = 1000;
    public final static int FUNC_CHECK_PERMISSION = FUNC_BASE + 1;
    public final static int FUNC_CPU_HIGH_FREQ = FUNC_BASE + 2;
    public final static int FUNC_CANCEL_CPU_HIGH_FREQ = FUNC_BASE + 3;
    public final static int FUNC_CPU_CORE_FOR_THREAD = FUNC_BASE + 4;
    public final static int FUNC_CANCEL_CPU_CORE_FOR_THREAD = FUNC_BASE + 5;
    public final static int FUNC_HIGH_IO_FREQ = FUNC_BASE + 6;
    public final static int FUNC_CANCEL_HIGH_IO_FREQ = FUNC_BASE + 7;
    public final static int FUNC_SET_SCREEN_RESOLUTION = FUNC_BASE + 8;
    public final static int FUNC_RESET_SCREEN_RESOLUTION = FUNC_BASE + 9;
    public final static int FUNC_REG_ANR_CALLBACK = FUNC_BASE + 10;
    public final static int FUNC_REG_PRELOAD_BOOT_RESOURCE = FUNC_BASE + 11;
    public final static int FUNC_TERMINATE_APP = FUNC_BASE + 12;
    public final static int FUNC_UNIFY_CPU_IO_THREAD_CORE = FUNC_BASE + 13;
    public final static int FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE = FUNC_BASE + 14;
    public final static int FUNC_REG_SYSTEM_EVENT_CALLBACK = FUNC_BASE + 15;
    public final static int FUNC_GPU_HIGH_FREQ = FUNC_BASE + 16;
    public final static int FUNC_CANCEL_GPU_HIGH_FREQ = FUNC_BASE + 17;
    public final static int FUNC_CONFIGURE = FUNC_BASE + 18;

    /**
     * return code, if ret less than zero, means error code
     */
    public final static int RET_OK = 0;

    /**
     * requestCpuHighFreq，requestHighIOFreq returns level n
     */
    public final static int RET_LEVEL_1 = 1;
    public final static int RET_LEVEL_2 = 2;
    public final static int RET_LEVEL_3 = 3;

    /**
     * requestUnifyCpuIOThreadCoreGpu use combine flags
     */
    //预留返回值最后三位作为level，倒数第四位代表cpu level，倒数第五位代表io level，新增值继续左移
    public final static int RET_CPU_HIGH_FREQ = 1 << 3;// 1000，即8
    public final static int RET_HIGH_IO_FREQ = 1 << 4; // 10000，即16
    public final static int RET_GPU_HIGH_FREQ = 1 << 5; // 100000，即32, GPU只有level 1和0，故GPU可以不使用level位，直接用100000表示level 1

    //使用复合标识位
    public final static int RET_CPU_HIGH_FREQ_LEVEL_1 = RET_CPU_HIGH_FREQ | RET_LEVEL_1;   //Unify接口返回cpu level 1，1000 | 01 = 1001
    public final static int RET_CPU_HIGH_FREQ_LEVEL_2 = RET_CPU_HIGH_FREQ | RET_LEVEL_2;   //Unify接口返回cpu level 2，1000 | 10 = 1010
    public final static int RET_CPU_HIGH_FREQ_LEVEL_3 = RET_CPU_HIGH_FREQ | RET_LEVEL_3;   //Unify接口返回cpu level 3，1000 | 11 = 1011

    public final static int RET_HIGH_IO_FREQ_LEVEL_1 = RET_HIGH_IO_FREQ | RET_LEVEL_1;     //Unify接口返回io level 1，10000 | 01 = 10001
    public final static int RET_HIGH_IO_FREQ_LEVEL_2 = RET_HIGH_IO_FREQ | RET_LEVEL_2;     //Unify接口返回io level 2，10000 | 10 = 10010
    public final static int RET_HIGH_IO_FREQ_LEVEL_3 = RET_HIGH_IO_FREQ | RET_LEVEL_3;     //Unify接口返回io level 3，10000 | 11 = 10011

    /**
     * error code from java
     */
    public final static int ERROR_CODE_SUCCESS = 0;
    public final static int ERROR_CODE_FAILED = -1;
    public final static int ERROR_CODE_CHECKENV_FAIL = -2;
    public final static int ERROR_CODE_NOT_ENABLE = -3;
    public final static int ERROR_CODE_NOT_FOREGROUND = -4;
    public final static int ERROR_CODE_HCPERFMANAGER_NULL = -5;
    public final static int ERROR_CODE_DISCONNECT= -6;

    /**
     * error code from native
     */
    public final static int CLIENT_CONNECT = -20000;
    public final static int CLIENT_DISCONNECT = -20001;
    public final static int ERR_CLIENT_ALREADY_INIT = -20002;
    public final static int ERR_UNAUTHORIZED = -10001;

    /**
     * APP SCENE, define by app
     */
    public final static int APP_SCENE_UNDEFINE = 0;                 //未定义场景
    public final static int APP_SCENE_STARTUP = 1;                  //进程启动，APP启动
    public final static int APP_SCENE_WINDOW_SWITCH = 2;            //UI页面切换（同一进程），activity，fragment切换
    public final static int APP_SCENE_WINDOW_SCROLL = 3;            //UI页面滑动
    public final static int APP_SCENE_DATA_LOADING_AND_PROCESS = 4; //数据加载和处理任务，指APP本地前后台任务
    public final static int APP_SCENE_MULTI_MEDIA_PROCESS = 5;      //多媒体相关业务
    public final static int APP_SCENE_COMMUNICATE = 6;              //APP/服务端通信
    public final static int APP_SCENE_SYSTEM_DEVICE = 7;            //调用系统服务

    /**
     * callback type for {@link #onData(int, long, long, int, int, int, byte[])}
     */
    public final static int CALLBACK_TYPE_BASE = 0;
    public final static int CALLBACK_TYPE_STATUS = CALLBACK_TYPE_BASE + 1;
    public final static int CALLBACK_TYPE_DATA = CALLBACK_TYPE_BASE + 2;


    public static int tickRate = 100;//default


    /**
     * whethet the socket connect success, set after call {@link #initHardCoder(String, int, String, HCPerfManagerThread, Context, HardCoderCallback.ConnectStatusCallback)}
     * and receive the callback {@link #onData(int, long, long, int, int, int, byte[])}
     */
    private static boolean connect = false;
    public boolean isConnect(){
      return  connect;
    }

    private static HCPerfManager hcPerfManager;
    private static byte[] lock = new byte[0];

    /**
     * whether the phone supports Hardcoder
     */
    private static boolean checkEnv = false;
    public static void setCheckEnv(boolean env){
        checkEnv = env;
    }
    public static boolean isCheckEnv() {
        return checkEnv;
    }

    /**
     * when hcDebug is true, hardcoder will change log level to D. use {@link #setHcDebug(boolean)} to set
     */
    private static boolean hcDebug = false;
    public static boolean isHcDebug(){
        return hcDebug;
    }
    public static void setHcDebug(boolean debug){
        hcDebug = debug;
        if(checkEnv) {
            setDebug(debug);
        }
    }

    /**
     * whether the app(client) supports hardcoder, use {@link #setHcEnable(boolean)} to set
     */
    private static boolean hcEnable = true;
    public static boolean isHcEnable() {
        return hcEnable;
    }
    public static void setHcEnable(boolean enable) {
        hcEnable = enable;
        if(checkEnv) {
            setHCEnable(enable);
        }
    }

    /**
     * init Hardcoder, call after set hcEnable/hcDebug values {@link #setHcEnable(boolean)}{@link #setHcDebug(boolean)}}
     * must call before {@link #startPerformance(int, int, int, int, int[], int, int, long, int, String)}
     * @param remote server socket name, default {@link #readServerAddr()}
     * @param port server port
     * @param local local socket name, default {@link #CLIENT_SOCK}
     * @param hcPerfManagerThread the thread to run HcPerfManager, could be null
     * @param callback callback the connect result
     * @return ret
     */
    public static int initHardCoder(String remote, int port, String local, HCPerfManagerThread hcPerfManagerThread,
                                    HardCoderCallback.ConnectStatusCallback callback){
        if (!isHcEnable()) {
            HardCoderLog.e(TAG, String.format("initHardCoder client not enable Hardcoder, hcEnable:%b, " +
                    "checkEnv:%b", isHcEnable(), isCheckEnv()));
            return HardCoderJNI.ERROR_CODE_NOT_ENABLE;
        }

        checkEnv = ((null == remote || remote.equals("")) ? false : true );

        synchronized (lock) {
            if (hcPerfManager == null) {
                if (hcPerfManagerThread == null) {
                    hcPerfManagerThread = new HCPerfManagerThread() {
                        @Override
                        public Thread newThread(Runnable r, String name, int priority) {
                            return new Thread(r, name);
                        }
                    };
                }
                hcPerfManager = new HCPerfManager(hcPerfManagerThread);
            }
        }
        tickRate = getTickRate();
        connectStatusCallback = callback;
        return initHardCoder(remote, port, local);
    }

    /**
     * app can distribute a thread to run HCPerfManager or just use default thread like {@link #initHardCoder(String, int, String, HCPerfManagerThread, HardCoderCallback.ConnectStatusCallback)}}
     */
    public interface HCPerfManagerThread{
        Thread newThread(Runnable runnable, String name, int priority);
    }

    /**
     * check whether this app can use hardcoder in this phone.
     * must call after {@link #initHardCoder(String, int, String, HCPerfManagerThread, HardCoderCallback.ConnectStatusCallback)}
     * after initHardCoder callback, to sure that socket is connected {@link #connect}
     * @param manufactures manufacture name list
     * @param certs certification string list that you get from the corresponding manufacture
     * @param callback callback when server return yes/no
     * @return request id
     */
    public static long checkPermission(String[] manufactures, String[] certs, HardCoderCallback.FuncRetCallback callback){
        if (!connect) {
            HardCoderLog.e(TAG, "startPerformance error disconnect");
            return ERROR_CODE_DISCONNECT;
        }
        long requestId = checkPermission(manufactures, certs, android.os.Process.myPid(), android.os.Process.myUid(), android.os.Process.myTid(), SystemClock.elapsedRealtimeNanos());
        funcRetCallback.put(requestId, callback);
        return requestId;
    }


    /**
     * if the request doesn't need to bind tids, use this function
     * other params the same as {@link #startPerformance(int, int, int, int, int[], int, int, long, int, String)}
     * @return request hashcode, will be used at {@link #stopPerformance(int)}
     */
    public static int startPerformance(final int delay, final int cpuLevel, final int ioLevel,
                                       final int tid, final int timeout, final int scene, final long action,
                                       final int callerTid, final String tag) {
        int[] tids = new int[1];
        tids[0] = tid;
        return startPerformance(delay, cpuLevel, ioLevel, GPU_LEVEL_0, tids, timeout, scene,
                action, callerTid, tag);
    }

    /**
     * request cpu/io/bind tids/gpu
     * call after {@link #initHardCoder(String, int, String, HCPerfManagerThread, HardCoderCallback.ConnectStatusCallback)}
     * @param delay ms, how long will the task delay to excute
     * @param cpuLevel request CPU level, {@link #CPU_LEVEL_0}
     * @param ioLevel request IO level, {@link #IO_LEVEL_0}
     * @param gpuLevel request GPU level, {@link #GPU_LEVEL_0}
     * @param tids the threads need to bind core
     * @param timeout ms, how long will the task expire
     * @param scene app scene, like {@link #APP_SCENE_STARTUP}
     * @param action app action, retain value, useless now
     * @param callerTid the thread call this task
     * @param tag tag
     * @return request hashcode, will be used at {@link #stopPerformance(int)}
     */
    public static int startPerformance(final int delay, final int cpuLevel, final int ioLevel, final int gpuLevel, final int[] tids,
                                       final int timeout, final int scene, final long action, final int callerTid, final String tag) {
        if (hcPerfManager == null) {
            HardCoderLog.e(TAG, "startPerformance error HCPerfManager is null, please " +
                    "initHardCoder first!");
            return ERROR_CODE_HCPERFMANAGER_NULL;
        }

        if (!connect) {
            HardCoderLog.e(TAG, "startPerformance error disconnect");
            return ERROR_CODE_DISCONNECT;
        }

        int hashCode = hcPerfManager.start(delay, cpuLevel, ioLevel, gpuLevel, tids,
                timeout, scene, action, callerTid, tag);
        HardCoderLog.i(TAG, String.format("hcPerfManager startPerformance:%d, scene:%d",
                hashCode, scene));
        return hashCode;
    }

    /**
     * call when the scene end
     * @param hashCode the return value for {@link #startPerformance(int, int, int, int, int[], int, int, long, int, String)}
     * @return ret, success {@link #ERROR_CODE_SUCCESS}, and fail {@link #ERROR_CODE_FAILED}
     */
    public static int stopPerformance(int hashCode) {
        if (hcPerfManager == null) {
            HardCoderLog.e(TAG, "stopPerformance error HCPerfManager is null, please initHardCoder first!");
            return ERROR_CODE_HCPERFMANAGER_NULL;
        }

        if (!connect) {
            HardCoderLog.e(TAG, "stopPerformance error disconnect");
            return ERROR_CODE_DISCONNECT;
        }
        boolean ret = hcPerfManager.stop(hashCode);

        HardCoderLog.i(TAG, String.format("stopPerformance:%d, ret:%b",
                hashCode, ret));
        return ret ? ERROR_CODE_SUCCESS : ERROR_CODE_FAILED;
    }

    /**
     * system callback C2JAVA
     * @param callbackType callbackType, {@link #CALLBACK_TYPE_BASE}
     * @param requestId request id
     * @param timestamp timestamp
     * @param retCode ret, whether success or fail
     * @param funcId function id, {@link #FUNC_BASE}
     * @param dataType callback buffer type
     * @param buffer return params
     */
    public static void onData(final int callbackType, final long requestId, final long timestamp, final int retCode,
                              final int funcId, final int dataType, final byte[] buffer) {
        HardCoderLog.i(TAG, "onData callbackType:" + callbackType + ", requestId:" + requestId +
                ", timestamp:" + timestamp + ", retCode:" + retCode +
                ", funcId:" + funcId + ", dataType:" + dataType);
        switch (callbackType) {
            case CALLBACK_TYPE_STATUS:
                switch (funcId) {
                    case FUNC_LOCAL:
                        switch(retCode){
                            case CLIENT_CONNECT:
                                connect = true;
                                if(connectStatusCallback != null){
                                    connectStatusCallback.onConnectStatus(connect);
                                }
                                break;
                            case CLIENT_DISCONNECT:
                                connect = false;
                                if(connectStatusCallback != null){
                                    connectStatusCallback.onConnectStatus(connect);
                                }
                                break;
                            case ERR_CLIENT_ALREADY_INIT:
                                connect = true;
                                HardCoderLog.i(TAG, "onData callback, already initHardcoder!");
                                break;
                        }
                        break;
                    default:
                        onRequestCallback(callbackType, requestId, retCode, funcId, dataType, buffer);
                        break;
                }
                break;
            case CALLBACK_TYPE_DATA: {
                switch (funcId) {
                    case FUNC_REG_ANR_CALLBACK:
                        if (anrCallback != null) {
                            anrCallback.onANR(buffer);
                        }
                        break;
                    default:
                        onRequestCallback(callbackType, requestId, retCode, funcId, dataType, buffer);
                        break;
                }
                break;
            }
            default:
                onRequestCallback(callbackType, requestId, retCode, funcId, dataType, buffer);
                break;
        }
    }


    /**
     * native function, only call when {@link #checkEnv} is true
     */
    private static native int initHardCoder(String remote, int port, String local);

    public static native long checkPermission(String[] manufactures, String[] certs, int pid, int uid,
                                             int tid, long timestamp);

    public static native long requestCpuHighFreq(int scene, long action, int level, int timeoutms,
                                                int tid, long timestamp);

    public static native long cancelCpuHighFreq(int tid, long timestamp);

    public static native long requestGpuHighFreq(int scene, long action, int level, int timeoutms,
                                                int tid, long timestamp);

    public static native long cancelGpuHighFreq(int tid, long timestamp);

    public static native long configure(byte[] data, int tid, long timestamp);

    public static native long requestCpuCoreForThread(int scene, long action, int[] bindtids,
                                                     int timeoutms, int tid, long timestamp);

    public static native long cancelCpuCoreForThread(int[] bindtids, int tid, long timestamp);

    public static native long requestHighIOFreq(int scene, long action, int level, int timeoutms,
                                               int tid, long timestamp);

    public static native long cancelHighIOFreq(int tid, long timestamp);

    public static native long requestScreenResolution(int level, String uiName, int tid, long timestamp);

    public static native long resetScreenResolution(int tid, long timestamp);

    public static native long registerANRCallback(int tid, long timestamp);

    public static native long registerSystemEventCallback(int tid, long timestamp);

    public static native long registerBootPreloadResource(String[] files, int tid, long timestamp);

    public static native long terminateApp(int tid, long timestamp);

    public static native long requestUnifyCpuIOThreadCoreGpu(int scene, long action, int cpulevel,
            int gpulevel, int iolevel, int[] bindtids, int timeoutms, int tid, long timestamp);

    public static native long cancelUnifyCpuIOThreadCoreGpu(boolean cancelcpu, boolean cancelgpu,
            boolean cancelio, boolean cancelthread, int[] bindtids, int tid, long timestamp);

    private static native void setHCEnable(boolean enable);

    public static native void setRetryConnectInterval(int interval);

    private static native void setDebug(boolean debug);

    public static native int startTraceCpuLoad(int linuxTid, int sampleRate);

    public static native int stopTraceCpuLoad(int id);

    public static native int isRunning();

    private static native int getTickRate();

    public static native long getParameters(byte[] data, int tid, long timestamp);


    public final static String SERVER_PROP_KEY = "persist.sys.hardcoder.name";
    public final static String CLIENT_SOCK = ".hardcoder.client.sock";
    private static volatile Class<?> sSystemPropertiesClazz = null;
    private static Method sGetStringPropsMethod = null;

    /**
     * Default implementation to read server socket name
     * @return server socket name. If null, means that this phone do not support hardcoder server.
     */
    public static String readServerAddr() {
        try {
            synchronized (lock) {
                if (sSystemPropertiesClazz == null) {
                    sSystemPropertiesClazz = Class.forName("android.os.SystemProperties");
                    sGetStringPropsMethod = sSystemPropertiesClazz.getDeclaredMethod("get",
                            String.class, String.class);
                    sGetStringPropsMethod.setAccessible(true);
                }
                final String line = (String) sGetStringPropsMethod.invoke(null,
                        HardCoderJNI.SERVER_PROP_KEY, "");
                HardCoderLog.i(TAG, String.format("readServerAddr, serverprop[%s] result[%s]",
                        SERVER_PROP_KEY, line));
                return line;
            }
        } catch (Throwable e) {
            HardCoderLog.printErrStackTrace(TAG, e, "readServerAddr");
            return "";
        }
    }


    /**
     * callback
     */
    private static HardCoderCallback.ConnectStatusCallback connectStatusCallback;
    private static HashMap<Long, HardCoderCallback.FuncRetCallback> funcRetCallback = new HashMap<>();
    private static HashMap<Long, HardCoderCallback.RequestStatus> requestStatusCallback = new HashMap<>();
    public static void putRequestStatusHashMap(long requestId, HardCoderCallback.RequestStatus requestStatus){
        if(requestStatusCallback != null){
            requestStatusCallback.put(requestId, requestStatus);
        }
    }

    public static void putFuncRetHashMap(long requestId, HardCoderCallback.FuncRetCallback callback){
        if(funcRetCallback != null){
            funcRetCallback.put(requestId, callback);
        }
    }

    public static void onRequestCallback(final int callbackType, final long requestId, final int retCode,
                                         final int funcId, final int dataType, final byte[] buffer){
        HardCoderCallback.RequestStatus requestStatus = null;
        HardCoderCallback.FuncRetCallback callback = null;
        if(requestStatusCallback != null){
            requestStatus = requestStatusCallback.remove(requestId);
        }
        if(funcRetCallback != null){
            callback = funcRetCallback.remove(requestId);
            if(callback != null) {
                callback.onFuncRet(callbackType, requestId, retCode, funcId, dataType, buffer);
            }
        }
        if(requestStatus != null || callback != null) {
            HardCoderLog.i(TAG, "onRequestCallback, funcId:" + funcId + ", requestId:" + requestId +
                    ", retCode:" + retCode + ", requestStatus:" +
                    (requestStatus == null ? "null" : requestStatus.toString()) + ", funcRetCallback:" +
                    (callback == null ? "false" : "true"));
        }
    }


    /**
     * ANR callback
     * {@link #onData(int, long, long, int, int, int, byte[])}
     */
    private static AnrCallback anrCallback;
    public interface AnrCallback {
        void onANR(byte[] log);
    }
    public static long registerANRCallback(AnrCallback callback) {
        anrCallback = callback;
        return registerANRCallback(Process.myTid(), SystemClock.elapsedRealtime());
    }

    /**
     * report callback
     */
    public static HardCoderCallback.SceneReportCallback sceneReportCallback;
    public static void setSceneReportCallback(HardCoderCallback.SceneReportCallback callback){
        sceneReportCallback = callback;
    }

}
