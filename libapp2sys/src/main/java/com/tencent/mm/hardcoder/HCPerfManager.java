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

import android.os.Process;
import android.os.SystemClock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

/**
 * Manage Hardcoder tasks.
 */
public class HCPerfManager {
    private final static String TAG = "Hardcoder.HCPerfManager";
    private final static int[] CPU_LEVEL = new int[]{HardCoderJNI.CPU_LEVEL_0, HardCoderJNI.CPU_LEVEL_1,
        HardCoderJNI.CPU_LEVEL_2, HardCoderJNI.CPU_LEVEL_3};
    private final static int[] IO_LEVEL = new int[]{HardCoderJNI.IO_LEVEL_0, HardCoderJNI.IO_LEVEL_1,
        HardCoderJNI.IO_LEVEL_2, HardCoderJNI.IO_LEVEL_3};

    public final static int CPU_LEVEL_CANCEL = -1;
    public final static int CPU_LEVEL_KEEP = -2;
    public final static int GPU_LEVEL_CANCEL = -1;
    public final static int GPU_LEVEL_KEEP = -2;
    public final static int IO_LEVEL_CANCEL = -1;
    public final static int IO_LEVEL_KEEP = -2;


    private HCPerfStatThread hcPerfStatThread;
    private Thread hcPerfThread;
    private HardCoderJNI.HCPerfManagerThread hcPerfManagerThread;

    /**
     * tasks need to be stopped
     */
    public static class PerformanceTaskStop {
        public long sceneStopTime = 0L;
        public int hashCode = 0;
    }

    /**
     * tasks need to be added to the start queue
     */
    public static class PerformanceTask {
//        public long sceneStartTime = 0L;
        public long sceneStopTime = 0L;

        public int delay = 0;
        public int timeout = 0;
        public int cpuLevel = 0;
        public int gpuLevel = 0;
        public int ioLevel = 0;
        public int[] bindTids = null;
        public int scene = 0;
        public long action = 0;
        public long startTime = 0;
        public long initTime = 0;
        public long stopTime = 0;
        public int callerTid = 0;
        public String tag;

        public int lastCpuLevel = 0;
        public int lastIoLevel = 0;
        public long lastUpdateTime = 0;
        public int[] cpuLevelTimeArray = new int[CPU_LEVEL.length];
        public int[] ioLevelTimeArray = new int[IO_LEVEL.length];
        public int[] bindCoreThreadIdArray = null;
        public long averageCoreFreq = 0L;//jiffies/HZ * averageCoreFreq

        /**
         * check whether the task request to bind threads to the core
         * @return boolean
         */
        public boolean isNeedBindTids(){
            if(bindTids == null || bindTids.length <= 0 || (bindTids.length == 1 && bindTids[0] <= 0)){
                return false;
            }
            return true;
        }

        /**
         * print threads that need to bind to the core
         * @return thread string
         */
        public String bindTidsToString(){
            if(isNeedBindTids()) {
                final StringBuilder stringBuilder = new StringBuilder("[");
                for(int tid : bindTids){
                    stringBuilder.append("" + tid + " ");
                }
                stringBuilder.append("]");
                return stringBuilder.toString();
            } else {
                return "[ ]";
            }
        }

        public String toString(long baseTime) {
            return String.format("hashCode:%x time:[init:%d, start:%d, stop:%d][delay:%d, timeout:%d]" +
                            "[scene:%d, action:%d, callerTid:%d][cpu:%d, io:%d, gpu:%d] bindTids:%s [TAG:%s]",
                    hashCode(), initTime - baseTime, startTime - baseTime, stopTime - baseTime, delay, timeout,
                    scene, action, callerTid, cpuLevel, ioLevel, gpuLevel, bindTidsToString(), tag);
        }

        /**
         * for statics
         * @param updateCoreFreq new core frequency
         */
        public void update(long updateCoreFreq) {
            this.averageCoreFreq = (this.averageCoreFreq + updateCoreFreq) / 2;
        }
    }

    private LinkedBlockingQueue<Object> taskQueue = new LinkedBlockingQueue<>();

    /**
     * init the HCPerfManager
     * @param thread a thread
     */
    public HCPerfManager(HardCoderJNI.HCPerfManagerThread thread) {
        hcPerfStatThread = new HCPerfStatThread();
        hcPerfStatThread.start();

        hcPerfManagerThread = thread;

        hcPerfThread = hcPerfManagerThread.newThread(new HCPerfRunnable(), "HCPerfManager", Thread.MAX_PRIORITY);
        hcPerfThread.start();
        HardCoderLog.i(TAG, String.format("HCPerfManager new thread[%s]", hcPerfThread));
    }

    /**
     * check params
     * @param delay delay to excute
     * @param cpuLevel request cpu level
     * @param ioLevel request io level
     * @param gpuLevel request gpu level
     * @param bindTids threads request to bind core
     * @param timeout expire time
     * @return boolean
     */

    private boolean isParamsValid(final int delay, final int cpuLevel, final int ioLevel, final int gpuLevel,
                                  final int[] bindTids, final int timeout){
        if(delay < 0 || cpuLevel < 0 || gpuLevel < 0 || ioLevel < 0 || bindTids == null ||
                bindTids.length < 0 || timeout <= 0){
            return false;
        }
        return true;
    }

    /**
     * add tasks to queue
     * @param delay delay to excute
     * @param cpuLevel request cpu level
     * @param ioLevel request io level
     * @param gpuLevel request gpu level
     * @param bindTids threads request to bind core
     * @param timeout expire time
     * @param scene app scene
     * @param action app action
     * @param callerTid the thread call this request
     * @param tag request hashcode
     * @return request hashcode, will be used at {@link #stop(int)}
     */
    public int start(final int delay, final int cpuLevel, final int ioLevel, final int gpuLevel, final int[] bindTids,
                     final int timeout, final int scene, final long action, final int callerTid, final String tag) {

        if (!isParamsValid(delay,cpuLevel, ioLevel, gpuLevel, bindTids, timeout) ||
                (cpuLevel == 0 && ioLevel == 0 && gpuLevel == 0 && bindTids.length == 1 && bindTids[0] == 0)) {
            HardCoderLog.e(TAG, String.format("start error params, ignore this task! delay:%d, " +
                    "[cpu:%d, io:%d, gpu:%d], bindTids:%d, timeout:%d", delay, cpuLevel, ioLevel, gpuLevel,
                    (bindTids == null ? 0 : bindTids.length), timeout));
            return 0;
        }

        final PerformanceTask task = new PerformanceTask();
        task.delay = delay;
        task.cpuLevel = cpuLevel;
        task.ioLevel = ioLevel;
        task.bindTids = bindTids.clone();
        task.gpuLevel = gpuLevel;
        task.timeout = timeout;
        task.scene = scene;
        task.action = action;
        task.callerTid = callerTid;
        task.initTime = System.currentTimeMillis();
        task.startTime = task.initTime + delay;
        task.stopTime = task.initTime + delay + timeout;
        task.tag = tag;
        task.lastUpdateTime = task.startTime;

        final boolean ret = taskQueue.offer(task);

        HardCoderLog.d(TAG, String.format("start ret:%b ,task:%s", ret, task.toString(task.initTime)));
        return ret ? task.hashCode() : 0;
    }

    /**
     * remove the task from queue
     * @param hashCode get when call {@link #start(int, int, int, int, int[], int, int, long, int, String)}
     * @return success
     */
    public boolean stop(int hashCode) {
        PerformanceTaskStop stop = new PerformanceTaskStop();
        stop.sceneStopTime = System.currentTimeMillis();
        stop.hashCode = hashCode;
        final boolean ret = (hashCode != 0 ? taskQueue.offer(stop) : false);

        HardCoderLog.d(TAG, String.format("stop ret:%b, hashcode:%x", ret, hashCode));
        return ret;
    }

    private class HCPerfRunnable implements Runnable{
        private boolean running = false;
        // #lizard forgives
        @Override
        public void run() {
            HardCoderLog.i(TAG, String.format("HCPerfManager thread run start, id:%d, name:%s",
                    (Thread.currentThread() == null ? -1 : Thread.currentThread().getId()),
                    Thread.currentThread().getName()));
            running = true;

            PerformanceTask currCpuLevel = null;
            PerformanceTask currGpuLevel = null;
            PerformanceTask currIoLevel = null;
            HashSet<PerformanceTask> currBindCore = new HashSet<>();

            long nextWakeInterval = DEFAULT_WAKE_INTERVAL;
            final ArrayList<PerformanceTask> listStartTask = new ArrayList<>();
            long[] startProcessJiffies = HardCoderUtil.getMyProcCpuTime();
            if (startProcessJiffies == null || startProcessJiffies.length < 2){
                startProcessJiffies = new long[] {0L, 0L};
                HardCoderLog.d(HardCoderReporter.TAG, "process jiffies info is invalid");
            }
            HardCoderLog.d(HardCoderReporter.TAG, "process:[" + startProcessJiffies[0] + "," +
                    startProcessJiffies[1] + "]");

            final HashMap<Integer, PerformanceTaskStop> setStopTask = new HashMap<>();
            final HashSet<Integer> setStartedTask = new HashSet<>();

            while (running) {
                final long startLoop = System.currentTimeMillis();

                final int queueSize = taskQueue.size();

                HardCoderLog.d(TAG, "startLoop queue:" + queueSize + " startTask:" + listStartTask.size() +
                        " nextWakeInterval:" + nextWakeInterval);

                // if queue is empty , nextWakeInterval for wait
                for (int i = 0; i < (queueSize == 0 ? 1 : queueSize); i++) {
                    Object object = null;
                    try {
                        object = taskQueue.poll(nextWakeInterval, TimeUnit.MILLISECONDS);
                    } catch (Exception e) {
                        HardCoderLog.e(TAG, "taskQueue poll: " + e.getMessage());
                        object = null;
                    }
                    if (object == null){
                        break;
                    } else if (object instanceof PerformanceTask) {
                        listStartTask.add((PerformanceTask) object);
                        setStartedTask.add(((PerformanceTask) object).hashCode());
                    } else if (object instanceof PerformanceTaskStop) {
                        int hashCode = ((PerformanceTaskStop) object).hashCode;
                        if (setStartedTask.contains(hashCode)) {
                            setStopTask.put(hashCode, (PerformanceTaskStop) object);
                        }
                    } else {
                        if (HardCoderJNI.isCheckEnv()) {
                            assertFail("taskQueue poll invalid object");
                        } else {
                            uninit();
                            break;
                        }
                    }
                }

                nextWakeInterval = DEFAULT_WAKE_INTERVAL;
                PerformanceTask maxCpuLevel = null;
                PerformanceTask maxGpuLevel = null;
                PerformanceTask maxIoLevel = null;
                HashSet<PerformanceTask> needBindCore = new HashSet<>();
                final long now = System.currentTimeMillis();

                HardCoderLog.d(TAG, "InLoop startSize:" + listStartTask.size());

                for (int i = 0; i < listStartTask.size(); i++) {
                    final PerformanceTask task = listStartTask.get(i);
                    final long curProcessTime = System.currentTimeMillis();
                    if (task == null){
                        continue;
                    }

                    if (setStopTask.containsKey(task.hashCode())) {
                        task.stopTime = curProcessTime;
                        task.sceneStopTime = setStopTask.get(task.hashCode()).sceneStopTime;
                        setStopTask.remove(task.hashCode());
                        //finished report task
                        savePerformanceStatus(curProcessTime, listStartTask, CPU_LEVEL_KEEP, GPU_LEVEL_KEEP,
                                IO_LEVEL_KEEP, null);
                        reportPerformanceTask(task);
                    }

                    final long lifeLeftInterval = task.stopTime - curProcessTime;
                    if (lifeLeftInterval <= 0) {
                        HardCoderLog.d(TAG, "InLoop STOP:" + i + "/" + listStartTask.size() + " task:" +
                                task.toString(curProcessTime));
                        listStartTask.remove(task);
                        setStartedTask.remove(task.hashCode());
                        i--;
                        continue;
                    }
                    nextWakeInterval = Math.min(nextWakeInterval, lifeLeftInterval);

                    final long waitStartInterval = task.startTime - curProcessTime;
                    if (waitStartInterval > 0) {  //  task waiting start
                        HardCoderLog.d(TAG, "InLoop WAIT:" + i + "/" + listStartTask.size() + " task:" +
                                task.toString(curProcessTime));
                        nextWakeInterval = Math.min(nextWakeInterval, waitStartInterval);
                        continue;
                    }

                    HardCoderLog.d(TAG, "InLoop RUN:" + i + "/" + listStartTask.size() + " task:" +
                            task.toString(curProcessTime));

                    if (task.cpuLevel > 0
                            && ((maxCpuLevel == null) || (maxCpuLevel.cpuLevel > task.cpuLevel) ||
                            (maxCpuLevel.cpuLevel == task.cpuLevel && maxCpuLevel.stopTime < task.stopTime))) {
                        maxCpuLevel = task;
                    }

                    if (task.gpuLevel > 0
                            && ((maxGpuLevel == null) || (maxGpuLevel.gpuLevel > task.gpuLevel) ||
                            (maxGpuLevel.gpuLevel == task.gpuLevel && maxGpuLevel.stopTime < task.stopTime))) {
                        maxGpuLevel = task;
                    }

                    if (task.ioLevel > 0
                            && ((maxIoLevel == null) || (maxIoLevel.ioLevel > task.ioLevel) ||
                            (maxIoLevel.ioLevel == task.ioLevel && maxIoLevel.stopTime < task.stopTime))) {
                        maxIoLevel = task;
                    }

                    for(int tid : task.bindTids){
                        if(tid <= 0) {
                            continue;
                        }
                        needBindCore.add(task);
                        break;
                    }
                }

                final long endLoop = System.currentTimeMillis();

                HardCoderLog.d(TAG, String.format("EndLoop time:[%d,%d] list:%d stop:%d bindCore:%d -> %d",
                        endLoop - now, endLoop - startLoop, listStartTask.size(), setStopTask.size(),
                        currBindCore.size(), needBindCore.size()));
                HardCoderLog.d(TAG, "EndLoop CurrCpu:" + (currCpuLevel == null ? "null" : currCpuLevel.toString(now)) +
                        " -> MaxCpu:" + (maxCpuLevel == null ? "null" : maxCpuLevel.toString(now)));
                HardCoderLog.d(TAG, "EndLoop CurrGpu:" + (currGpuLevel == null ? "null" : currGpuLevel.toString(now)) +
                        " -> MaxGpu:" + (currGpuLevel == null ? "null" : currGpuLevel.toString(now)));
                HardCoderLog.d(TAG, "EndLoop CurrIO:" + (currIoLevel == null ? "null" : currIoLevel.toString(now)) +
                        " -> MaxIO:" + (maxIoLevel == null ? "null" : maxIoLevel.toString(now)));
                HardCoderLog.d(TAG, String.format("EndLoop BindCore.size cur: %d, need: %d", currBindCore.size(),
                        needBindCore.size()));

                HashSet<PerformanceTask> stopSet = new HashSet<>();
                stopSet.addAll(currBindCore);
                stopSet.removeAll(needBindCore);

                //check the current level
                //if current level is the same with the request, continue
                if (maxCpuLevel == currCpuLevel && maxGpuLevel == currGpuLevel && maxIoLevel == currIoLevel &&
                        currBindCore.size() == needBindCore.size() && stopSet.isEmpty()) {//same
                    HardCoderLog.d(TAG, "EndLoop Nothing Changed, Continue.");
                    continue;
                }

                int reqCpuLevel = 0;
                int reqGpuLevel = 0;
                int reqIOLevel = 0;
                int reqScene = 0;
                long reqAction = 0;
                int reqCallerTid = 0;
                int reqTimeStamp = Integer.MAX_VALUE;

                int cancelTidCount = 0;//need unbind tids count
                for(PerformanceTask performanceTask : stopSet){
                    if(performanceTask.bindTids == null) {
                        continue;
                    }
                    cancelTidCount += performanceTask.bindTids.length;
                }

                if (cancelTidCount > 0) {
                    int[] arrCancelTid = new int[cancelTidCount];
                    int idxStop = 0;
                    for (Iterator<PerformanceTask> it = stopSet.iterator(); it.hasNext(); ) {
                        PerformanceTask task = it.next();
                        final long curProcessTime = System.currentTimeMillis();
                        HardCoderLog.i(TAG, "!cancelBindCore task:" + task.toString(curProcessTime));
                        if (task.stopTime > curProcessTime) {
                            HardCoderLog.e(TAG, "stopTime:" + (task.stopTime - curProcessTime) + ". Error !");
                            continue;
                        }
                        if (!task.isNeedBindTids()) {
                            if (HardCoderJNI.isCheckEnv()) {
                                assertFail("bindTids:" + task.bindTidsToString());
                            }else {
                                uninit();
                                break;
                            }
                        }
                        for(int tid : task.bindTids){
                            arrCancelTid[idxStop] = tid;
                            idxStop++;
                        }
                    }
                    if (checkEnv()) {
                        HardCoderJNI.cancelCpuCoreForThread(arrCancelTid, Process.myTid(),
                                SystemClock.elapsedRealtimeNanos());
                    }
                    savePerformanceStatus(now, listStartTask, CPU_LEVEL_KEEP, GPU_LEVEL_KEEP, IO_LEVEL_KEEP, null);
                }

                currBindCore = needBindCore; // always include all

                int tidsLen = 0;//need bind tids count
                for(PerformanceTask performanceTask : currBindCore){
                    if(performanceTask.bindTids == null) {
                        continue;
                    }
                    tidsLen += performanceTask.bindTids.length;
                }

                int[] tidList = new int[tidsLen];
                int idxStart = 0;
                for (Iterator<PerformanceTask> it = currBindCore.iterator(); it.hasNext(); ) {
                    PerformanceTask task = it.next();
                    HardCoderLog.i(TAG, "requestBindCore task:" + task.toString(now));
                    final long curProcessTime = System.currentTimeMillis();

                    if (task.stopTime <= curProcessTime) {
                        HardCoderLog.e(TAG, "stopTime:" + (task.stopTime - curProcessTime) + ". Error !");
                        continue;
                    }

                    if (!task.isNeedBindTids()) {
                        if (HardCoderJNI.isCheckEnv()) {
                            assertFail("bindTids:" + task.bindTidsToString());
                        }else {
                            uninit();
                            break;
                        }
                    }
                    for(int tid : task.bindTids) {
                        tidList[idxStart] = tid;
                        idxStart++;
                    }
                    reqScene = task.scene;
                    reqAction = task.action;
                    reqCallerTid = task.callerTid;
                    reqTimeStamp = Math.min((int) (task.stopTime - now), reqTimeStamp);
                }

                if (maxIoLevel == null) { // here curr should be timeout.
                    if (currIoLevel != null) {
                        HardCoderLog.i(TAG, "!cancelHighIOFreq task:" + currIoLevel.toString(now));
                        if (checkEnv()) {
                            HardCoderJNI.cancelHighIOFreq(currIoLevel.callerTid, SystemClock.elapsedRealtimeNanos());
                        }
                        savePerformanceStatus(now, listStartTask, CPU_LEVEL_KEEP, GPU_LEVEL_KEEP,
                                IO_LEVEL_CANCEL, null);
                    }
                } else if (currIoLevel != maxIoLevel) { //this is a new level
                    HardCoderLog.d(TAG, "IOReq:" + (currIoLevel == null ? "null" : currIoLevel.toString(now)) +
                                " -> " + maxIoLevel.toString(now) + " delay:" + (endLoop - maxIoLevel.startTime));
                }
                currIoLevel = maxIoLevel; // always use max
                if (currIoLevel != null) {
                    reqIOLevel = currIoLevel.ioLevel;
                    reqScene = currIoLevel.scene;
                    reqAction = currIoLevel.action;
                    reqCallerTid = currIoLevel.callerTid;
                    reqTimeStamp = Math.min((int) (currIoLevel.stopTime - now), reqTimeStamp);
                }

                if (maxGpuLevel == null) { // here curr should be timeout.
                    if (currGpuLevel != null) {
                        HardCoderLog.i(TAG, "!cancelGpuHighFreq task:" + currGpuLevel.toString(now));
                        if (checkEnv()) {
                            HardCoderJNI.cancelGpuHighFreq(currGpuLevel.callerTid, SystemClock.elapsedRealtimeNanos());
                        }
                        savePerformanceStatus(now, listStartTask, CPU_LEVEL_KEEP, GPU_LEVEL_CANCEL, IO_LEVEL_KEEP,
                                null);
                    }
                } else if (currGpuLevel != maxGpuLevel) { //this is a new level
                    HardCoderLog.d(TAG, "GPUReq:" + (currGpuLevel == null ? "null" : currGpuLevel.toString(now)) +
                                " -> " + maxGpuLevel.toString(now) + " delay:" + (endLoop - maxGpuLevel.startTime));
                }
                currGpuLevel = maxGpuLevel; // always use max
                if (currGpuLevel != null) {
                    reqGpuLevel = currGpuLevel.gpuLevel;
                    reqScene = currGpuLevel.scene;
                    reqAction = currGpuLevel.action;
                    reqCallerTid = currGpuLevel.callerTid;
                    reqTimeStamp = Math.min((int) (currGpuLevel.stopTime - now), reqTimeStamp);
                }

                if (maxCpuLevel == null) { // here curr should be timeout.
                    if (currCpuLevel != null) {
                        HardCoderLog.i(TAG, "!cancelCpuHighFreq task:" + currCpuLevel.toString(now));
                        if (checkEnv()) {
                            HardCoderJNI.cancelCpuHighFreq(currCpuLevel.callerTid, SystemClock.elapsedRealtimeNanos());
                        }
                        savePerformanceStatus(now, listStartTask, CPU_LEVEL_CANCEL, GPU_LEVEL_KEEP, IO_LEVEL_KEEP,
                                null);
                    }
                } else if (currCpuLevel != maxCpuLevel) {
                    HardCoderLog.d(TAG, "CPUReq:" + (currCpuLevel == null ? "null" : currCpuLevel.toString(now)) +
                                " -> " + maxCpuLevel.toString(now) + " delay:" + (endLoop - maxCpuLevel.startTime));
                }
                currCpuLevel = maxCpuLevel;//always use max
                if (currCpuLevel != null) {
                    reqCpuLevel = currCpuLevel.cpuLevel;
                    reqScene = currCpuLevel.scene;
                    reqAction = currCpuLevel.action;
                    reqCallerTid = currCpuLevel.callerTid;
                    reqTimeStamp = Math.min((int) (currCpuLevel.stopTime - now), reqTimeStamp);
                }

                if (reqTimeStamp < Integer.MAX_VALUE) {
                    HardCoderLog.i(TAG, String.format("!UnifyRequest [%d,%d,%d] [%d,%d,%d,%d] TO:%d max CPU:%s " +
                            "GPU:%s IO:%s cur CPU:%s GPU:%s IO:%s", reqScene, reqAction, reqCallerTid,
                            reqCpuLevel, reqGpuLevel, reqIOLevel, tidList.length, reqTimeStamp,
                            maxCpuLevel == null ? "null" : maxCpuLevel.toString(now),
                            maxGpuLevel == null ? "null" : maxGpuLevel.toString(now),
                            maxIoLevel == null ? "null" : maxIoLevel.toString(now),
                            currCpuLevel == null ? "null" : currCpuLevel.toString(now),
                            currGpuLevel == null ? "null" : currGpuLevel.toString(now),
                            currIoLevel == null ? "null" : currIoLevel.toString(now)
                    ));

                    assertTrue(reqTimeStamp > 0);
                    assertTrue(reqScene > 0 || reqAction > 0);
                    assertTrue(reqCallerTid > 0);
                    assertTrue(reqCpuLevel > 0 || reqGpuLevel > 0 || reqIOLevel > 0 || tidList.length > 0);

                    if (checkEnv()){
                        long requestId = HardCoderJNI.requestUnifyCpuIOThreadCoreGpu(reqScene, reqAction, reqCpuLevel, reqGpuLevel,
                                reqIOLevel, tidList, reqTimeStamp, reqCallerTid, SystemClock.elapsedRealtimeNanos());
                        HardCoderJNI.putRequestStatusHashMap(requestId, new HardCoderCallback.RequestStatus(reqScene, reqAction,
                                reqCpuLevel, reqGpuLevel, reqIOLevel, tidList));
                        HardCoderLog.i(TAG, "hardcoder requestUnifyCpuIOThreadCoreGpu requestId:" + requestId + "reqScene[" + reqScene + ", " +
                                        reqAction + "] running[enable:" + HardCoderJNI.isHcEnable() + ", env:" +
                                (HardCoderJNI.isCheckEnv() && HardCoderJNI.isRunning() > 0)+ "]");
                        if(HardCoderJNI.sceneReportCallback != null){
                            HardCoderJNI.sceneReportCallback.sceneReport(reqScene, reqAction);
                        }
                    }

                    savePerformanceStatus(now, listStartTask, reqCpuLevel, reqGpuLevel, reqIOLevel, tidList);
                }
            }
            HardCoderLog.i(TAG, "HCPerfManager thread run end.");
        }

        //kill task
        public void uninit() {
            running = false;
        }

        //restart new thread
        public void restart() {
            running = false;
            hcPerfThread.interrupt();

            hcPerfThread = hcPerfManagerThread.newThread(new HCPerfRunnable(), "HCPerfManager", Thread.MAX_PRIORITY);
            hcPerfThread.start();
            HardCoderLog.i(TAG, String.format("hardcoder HCPerfManager restart new thread[%s]", hcPerfThread));
        }

        private boolean checkEnv() {
            return HardCoderJNI.isCheckEnv();
        }
    }

    private final static long DEFAULT_WAKE_INTERVAL = 30 * 1000;//30s


    /**
     * for statics
     * @param time
     * @param listStartTask
     * @param reqCpuLevel
     * @param reqGpuLevel
     * @param reqIoLevel
     * @param bindCoreThreadIdArray
     */
    private void savePerformanceStatus(long time, List<PerformanceTask> listStartTask, int reqCpuLevel,
                                       int reqGpuLevel, int reqIoLevel, int[] bindCoreThreadIdArray) {
        this.hcPerfStatThread.addObject(new HCPerfStatThread.PerformanceStatus(time,
                new ArrayList<>(listStartTask), reqCpuLevel, reqGpuLevel, reqIoLevel, bindCoreThreadIdArray));
    }

    /**
     * for statics
     * @param performanceTask
     */
    private void reportPerformanceTask(PerformanceTask performanceTask) {
        HardCoderLog.d(TAG, "reportPerformanceTask:" + performanceTask.hashCode());
        this.hcPerfStatThread.addObject(performanceTask);
    }

    private static void assertTrue(String msg, boolean cond) {
        if (!cond) {
            assertFail(msg);
        }
    }

    private static void assertTrue(boolean cond) {
        assertTrue(null, cond);
    }
    
    private static void assertFail(String msg) {
        if (msg == null) {
            throw new AssertionError();
        }
        throw new AssertionError(msg);
    }
}
