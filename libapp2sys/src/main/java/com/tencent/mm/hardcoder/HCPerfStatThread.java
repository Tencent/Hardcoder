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

import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class HCPerfStatThread implements Runnable {

    private final static String TAG = "Hardcoder.HCPerfStatThread";
    private Queue<Object> objectQueue = new ConcurrentLinkedQueue<>();
    private Thread thread = null;
    private boolean needToInterrupt = false;

    /**
     * start
     */
    public synchronized void start() {
        if (this.thread == null) {
            this.thread = new Thread(this);
            this.thread.setPriority(Thread.NORM_PRIORITY);
            this.thread.start();
        }
    }

    /**
     * interrupt
     */
    public void interrupt() {
        this.needToInterrupt = true;
        synchronized (this) {
            this.notify();
        }
    }

    /**
     * real interrupt
     */
    private void realInterrupt() {
        if (this.thread != null) {
            this.thread.interrupt();
            this.thread = null;
            this.needToInterrupt = false;
        }
    }

    /**
     * run
     */
    public void run() {
        HardCoderLog.i(TAG, "HCPerfStatThread start to run.");
        if(Thread.currentThread() == null){
            HardCoderLog.e(TAG, "run error, current thread is null!");
            return;
        }
        while (Thread.currentThread() != null && !Thread.currentThread().isInterrupted()) {
            try {
                if (!this.objectQueue.isEmpty()) {
                    Object object = this.objectQueue.poll();
                    if (object instanceof PerformanceStatus) {
                        PerformanceStatus performanceStatus = (PerformanceStatus) object;
                        savePerformanceStatus(performanceStatus.time, performanceStatus.listStartTask,
                                performanceStatus.reqCpuLevel, performanceStatus.reqGpuLevel,
                                performanceStatus.reqIoLevel, performanceStatus.bindCoreThreadIdArray);
                    } else if (object instanceof HCPerfManager.PerformanceTask) {
                        HCPerfManager.PerformanceTask performanceTask = (HCPerfManager.PerformanceTask) object;
                        HardCoderReporter.performanceReport(performanceTask);
                    }
                } else {
                    synchronized (this) {
                        //check for the scene which notify first,so do it in synchronized block
                        if (this.needToInterrupt) {
                            this.realInterrupt();
                        }
                        while (this.objectQueue.isEmpty()) {
                            this.wait();
                        }
                    }
                }
            } catch (InterruptedException e) {
                HardCoderLog.e(TAG, "Performance status thread need to interrupt:" + e.getMessage());
                Thread.currentThread().interrupt();
                break;
            } catch (Exception e) {
                HardCoderLog.printErrStackTrace(TAG, e, "run exception:");
            }
        }
    }

    /**
     * add object
     * @param object different type to report
     */
    public void addObject(Object object) {
        if (this.objectQueue != null) {
            synchronized (this) {
                this.objectQueue.add(object);
                this.notify();
            }
        }
    }

    /**
     * finalize
     *
     * @throws Throwable
     */
    protected void finalize() throws Throwable {
        super.finalize();
        this.objectQueue.clear();
    }

    public static class PerformanceStatus {
        public final long time;
        public final List<HCPerfManager.PerformanceTask> listStartTask;
        public final int reqCpuLevel;
        public final int reqGpuLevel;
        public final int reqIoLevel;
        public final int[] bindCoreThreadIdArray;

        public PerformanceStatus(long time, List<HCPerfManager.PerformanceTask> listStartTask, int reqCpuLevel,
                                 int reqGpuLevel, int reqIoLevel, int[] bindCoreThreadIdArray) {
            this.time = time;
            this.listStartTask = listStartTask;
            this.reqCpuLevel = reqCpuLevel;
            this.reqGpuLevel = reqGpuLevel;
            this.reqIoLevel = reqIoLevel;
            this.bindCoreThreadIdArray = bindCoreThreadIdArray;
        }
    }

    // #lizard forgives
    private void savePerformanceStatus(long time, List<HCPerfManager.PerformanceTask> listStartTask,
                                       int reqCpuLevel, int reqGpuLevel, int reqIoLevel, int[] bindCoreThreadIdArray) {
        HardCoderLog.d(HardCoderReporter.TAG, String.format("forgives, time:%s, size:%s, " +
                        "cpu:%s, io:%s", time, listStartTask.size(), reqCpuLevel, reqIoLevel));
        for (int i = 0; i < listStartTask.size(); i++) {
            final HCPerfManager.PerformanceTask task = listStartTask.get(i);
            if (!task.isNeedBindTids()) {
                continue;
            }
            final long elapsedTime = time - task.lastUpdateTime;
            task.lastUpdateTime = time;
            if (reqCpuLevel != HCPerfManager.CPU_LEVEL_CANCEL) {
                if (reqCpuLevel == HCPerfManager.CPU_LEVEL_KEEP) {
                    task.cpuLevelTimeArray[task.lastCpuLevel] += elapsedTime;
                } else {//reqCpuLevel >= 0
                    task.lastCpuLevel = reqCpuLevel;
                    task.cpuLevelTimeArray[reqCpuLevel] += elapsedTime;
                }
            } else {//for cancel
                task.lastCpuLevel = HardCoderJNI.CPU_LEVEL_0;
                task.cpuLevelTimeArray[task.lastCpuLevel] += elapsedTime;
            }
            if (reqIoLevel != HCPerfManager.IO_LEVEL_CANCEL) {
                if (reqIoLevel == HCPerfManager.IO_LEVEL_KEEP) {
                    task.ioLevelTimeArray[task.lastIoLevel] += elapsedTime;
                } else {//reqIoLevel >= 0
                    task.lastIoLevel = reqIoLevel;
                    task.ioLevelTimeArray[reqIoLevel] += elapsedTime;
                }
            } else {//for cancel
                task.lastIoLevel = HardCoderJNI.IO_LEVEL_0;
                task.ioLevelTimeArray[task.lastIoLevel] += elapsedTime;
            }
            if (bindCoreThreadIdArray != null && bindCoreThreadIdArray.length > 0) {
                task.bindCoreThreadIdArray = bindCoreThreadIdArray;
            }

            final int coreId = HardCoderUtil.getThreadCoreId(task.isNeedBindTids() ? task.bindTids[0] : 0);
            final long coreFreq = HardCoderUtil.getCpuFreqByCoreId(coreId);
            if (task.averageCoreFreq == 0) {
                task.averageCoreFreq = coreFreq;
            }
            task.update(coreFreq);
        }
    }


}
