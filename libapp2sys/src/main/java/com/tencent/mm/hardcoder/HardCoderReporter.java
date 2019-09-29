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

/**
 * report Hardcoder performance
 * use {@link #setReporter(HardCoderReporterInterface)} if need report
 */
public class HardCoderReporter {
    public final static String TAG = "Hardcoder.HardCoderReporter";
    private static HardCoderReporterInterface reporter = null;

    private HardCoderReporter() {
    }

    public static void setReporter(HardCoderReporterInterface reporterImpl) {
        if (reporter == null) {
            HardCoderLog.i(TAG, String.format("setReporter[%s]", reporterImpl));
            reporter = reporterImpl;
        }
    }

    /**
     * report implementation
     * @param performanceTask request task
     */
    public static void performanceReport(HCPerfManager.PerformanceTask performanceTask) {
        int runtime = (int) (performanceTask.sceneStopTime - performanceTask.initTime);
        int enable = HardCoderJNI.isHcEnable() ? 1 : 0;//whether app support Hardcoder
        final int engineStatus = HardCoderJNI.isRunning();//whether device support Hardcoder
        int cancelInDelay = (runtime - performanceTask.delay) <= 0 ? 1 : 0;
        int scene = performanceTask.scene;
        long action = performanceTask.action;
        int cpuLevel = performanceTask.cpuLevel;
        int ioLevel = performanceTask.ioLevel;
        int[] bindCoreThreadIdArray = performanceTask.bindCoreThreadIdArray;
        int executeTime = (int) (performanceTask.stopTime - performanceTask.startTime);
        int phoneHZ = HardCoderJNI.tickRate;
        int[] cpuLevelTimeArray = performanceTask.cpuLevelTimeArray;
        int[] ioLevelTimeArray = performanceTask.ioLevelTimeArray;
        StringBuilder bindCoreThreadIdStringBuilder = new StringBuilder();
        if (bindCoreThreadIdArray != null) {
            for (int bindCoreThreadId : bindCoreThreadIdArray) {
                bindCoreThreadIdStringBuilder.append(bindCoreThreadId + "#");
            }
        }
        StringBuilder cpuLevelTimeStringBuilder = new StringBuilder();
        if (cpuLevelTimeArray != null) {
            for (int cpuLevelTime : cpuLevelTimeArray) {
                cpuLevelTimeStringBuilder.append(cpuLevelTime + "#");
            }
        }
        StringBuilder ioLevelTimeStringBuilder = new StringBuilder();
        if (ioLevelTimeArray != null) {
            for (int ioLevelTime : ioLevelTimeArray) {
                ioLevelTimeStringBuilder.append(ioLevelTime + "#");
            }
        }
        HardCoderLog.d(TAG, String.format("performanceReport, hash:%s, threadId:%s, enable:%s, " +
                "engineStatus:%s, cancelInDelay:%s, scene:%s, action:%s, lastCpuLevel:%s, cpuLevel:%s, " +
                "lastIoLevel:%s, ioLevel:%s, bindCoreIds:%s, executeTime:%s, runtime:%s, " +
                "phoneHZ:%s, cpuLevelTimeArray:%s, ioLevelTimeArray:%s",
                performanceTask.hashCode(), performanceTask.bindTidsToString(),
                enable, engineStatus, cancelInDelay, scene, action, performanceTask.lastCpuLevel,
                cpuLevel, performanceTask.lastIoLevel, ioLevel, bindCoreThreadIdStringBuilder.toString(),
                executeTime, runtime, phoneHZ, cpuLevelTimeStringBuilder.toString(),
                ioLevelTimeStringBuilder.toString()));
        
        if (reporter != null) {
            reporter.performanceReport(performanceTask.bindTids, enable, engineStatus, cancelInDelay, scene, action,
                    cpuLevel, ioLevel, bindCoreThreadIdArray, executeTime, runtime, phoneHZ, cpuLevelTimeArray, ioLevelTimeArray);
        }
    }

    public static void errorReport(int callbackType, long errorTimestamp, int errCode, int funcId, int dataType) {
        if (reporter != null) {
            reporter.errorReport(callbackType, errorTimestamp, errCode, funcId, dataType);
        }
    }

    public interface HardCoderReporterInterface {
        /**
         * performanceBindCoreThreadId 需要绑核的线程id
         * enable Java层是否打开 hc 开关 int
         * engineStatus 手机是否支持hc int
         * cancelInDelay 是否延迟中未执行就取消 int
         * scene 场景 int
         * action 操作 long
         * cpuLevel 请求的cpu level int
         * ioLevel 请求的IO level int
         * bindCoreThreadIdArray 绑核list text "#" 分割
         * executeTime 执行时间 int stop-start
         * runtime 运行时间 int stop-init
         * phoneHZ 手机时钟周期中断次数 int 单位时间内Tick节拍数，HardCoderJNI.tickRate
         * cpuLevelTimeArray 各个cpu level的时间 text "#"分割
         * ioLevelTimeArray 各个io level的时间 text "#"分割
         */

        /**
         * report implementation
         * @param performanceBindCoreThreadId threads need to bind core
         * @param enable whether app support Hardcoder
         * @param engineStatus whether device support Hardcoder
         * @param cancelInDelay true when the task has been canceled at delay time
         * @param scene scene
         * @param action action
         * @param cpuLevel request cpu level
         * @param ioLevel request io level
         * @param bindCoreThreadIdArray threads success to bind core
         * @param executeTime excute time, stopTime - startTime
         * @param runtime run time, stopTime - initTime
         * @param phoneHZ HardCoderJNI.tickRate
         * @param cpuLevelTimeArray all cpu level stay time
         * @param ioLevelTimeArray all io level stay time
         */
        void performanceReport(int[] performanceBindCoreThreadId, int enable, int engineStatus, int cancelInDelay,
                               int scene, long action, int cpuLevel, int ioLevel, int[] bindCoreThreadIdArray,
                               int executeTime, int runtime, int phoneHZ, int[] cpuLevelTimeArray, int[] ioLevelTimeArray);

        /**
         * error report
         * @param callbackType type of callback
         * @param errorTimestamp timestamp when error occured
         * @param errCode error code
         * @param funcId the requst function id
         * @param dataType the callback data type
         */
        void errorReport(int callbackType, long errorTimestamp, int errCode, int funcId, int dataType);

    }

}
