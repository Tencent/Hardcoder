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

package com.tencent.mm.hardcoder.testapp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.tencent.mm.hardcoder.HardCoderCallback;
import com.tencent.mm.hardcoder.HardCoderJNI;
import com.tencent.mm.hardcoder.HardCoderLog;

import java.math.BigDecimal;

import static com.tencent.mm.hardcoder.testapp.PITest.pitest;

/**
 * test activity main
 * test native method in hardcoder
 */
public class MainActivity extends Activity {

    private final static String TAG = "hardcoder.MainActivity";

    /**
     * scene and action value for test
     */
    private final static int SCENE_TEST = 101;
    private final static long ACTION_TEST = 1 << 0;

    int lastHash = 0;

    //是否打开hc开关
    private Button hcSwitchBtn;
    public static boolean hcSwitch = true;

    // #lizard forgives
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        /**
         * test initHardCoder
         */
        ((Button) findViewById(R.id.init_hardCoder)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final String remote = HardCoderJNI.readServerAddr();
                        final int ret = HardCoderJNI.initHardCoder(remote, 0, HardCoderJNI.CLIENT_SOCK, null, new HardCoderCallback.ConnectStatusCallback() {
                                    @Override
                                    public void onConnectStatus(final boolean isConnect) {
                                        HardCoderLog.i(TAG, "initHardCoder callback, isConnectSuccess:" + isConnect);
                                        fv.post(new Runnable() {
                                            @Override
                                            public void run() {
                                                Toast.makeText(MainActivity.this, "initHardCoder callback, server socket name:" +
                                                        remote + ", isConnectSuccess:" + isConnect, Toast.LENGTH_LONG).show();
                                            }
                                        });
                                    }
                                });
                        HardCoderLog.i(TAG, "initHardCoder, server socket name:" + remote);
                    }
                }).start();
            }
        });


        /**
         * test checkPermission
         * manufacture and certifacaion are arrays
         */
        ((Button) findViewById(R.id.check_permission)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                //manufacture为厂商
                //cert为testapp在OPPO手机上生成的key,具体值请向厂商申请生成
                //manufacture和cert均为数组，可同时传多个厂商的值
                String[] manufactures = {"OPPO"};
                String[] certs = {"gAnwcqfqFGGwTgX2MwdiBwikz9o8N/Tggq7pC6FuUaWME6/FbcMgFZFsFJR47P0Wo9My6" +
                        "cVYjotaa7njDlrGBIQ1PeSGPxiSN1LaC2tpRcOTzU8K1eUry3x/A0FdzM/gt+g9TEtuTH4WvhjVf3f" +
                        "qbnQjKrSl1jsOKIjn/4FtWsg="};
                final long requestId = HardCoderJNI.checkPermission(manufactures, certs,
                        new HardCoderCallback.FuncRetCallback() {
                        @Override
                        public void onFuncRet(int callbackType, long requestId, int retCode, int funcId, int dataType, byte[] buffer) {
                            final boolean isSuccess = (retCode == 0 ? true : false);
                            HardCoderLog.i(TAG, "checkpermission callback, isSuccess:" + isSuccess);
                            fv.post(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(MainActivity.this, "checkPermission, isSuccess:" + isSuccess, Toast.LENGTH_LONG).show();
                                }
                            });
                        }
                    });
                HardCoderLog.i(TAG, "checkPermission, requestId:" + requestId);
            }
        });

        //打开/关闭debug log
        //默认开
        HardCoderJNI.setHcDebug(true);
        ((Button) findViewById(R.id.debug_log)).setText(HardCoderJNI.isHcDebug() ? "debug_log：opened" : " debug_log：closed");
        ((Button) findViewById(R.id.debug_log)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                HardCoderJNI.setHcDebug(!HardCoderJNI.isHcDebug());
                ((Button) findViewById(R.id.debug_log)).setText(HardCoderJNI.isHcDebug() ?
                        "debug_log：opened" : " debug_log：closed");
            }
        });

        /**
         * test requestCpuHighFreq
         * native function
         * request highest cpu level
         */
        ((Button) findViewById(R.id.request_cpu_high_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final long requestId = hcSwitch ? HardCoderJNI.requestCpuHighFreq(SCENE_TEST, ACTION_TEST,
                                HardCoderJNI.CPU_LEVEL_1, 10000, android.os.Process.myTid(),
                                SystemClock.elapsedRealtimeNanos()) : 0;
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        for(int i = 0; i < 1000; i++){
                            pitest();
                        }
                        HardCoderLog.i(TAG, "requestCpuHighFreq, requestId:" + requestId + ", take " +
                                (System.currentTimeMillis() - s) + " ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, "requestCpuHighFreq, requestId:" + requestId, Toast.LENGTH_LONG).show();
                            }
                        });
                    }
                }).start();
            }
        });

        /**
         * test cancelCpuHighFreq
         * native function
         */
        ((Button) findViewById(R.id.cancel_cpu_high_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                final long requestId = HardCoderJNI.cancelCpuHighFreq(android.os.Process.myTid(),
                        SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "cancelCpuHighFreq, requestId:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "cancelCpuHighFreq, requestId:" + requestId, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

        /**
         * test requestGpuHighFreq
         * native function
         */
        ((Button) findViewById(R.id.request_gpu_high_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final long requestId = hcSwitch ? HardCoderJNI.requestGpuHighFreq(SCENE_TEST, ACTION_TEST,
                                HardCoderJNI.GPU_LEVEL_1, 10000, android.os.Process.myTid(),
                                SystemClock.elapsedRealtimeNanos()) : 0;
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        for(int i = 0; i < 1000; i++){
                            pitest();
                        }
                        HardCoderLog.i(TAG, "requestGpuHighFreq, requestId:" + requestId + " take " +
                                (System.currentTimeMillis() - s) + "ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, "requestGpuHighFreq, requestId:" + requestId,
                                        Toast.LENGTH_LONG).show();
                            }
                        });
                    }
                }).start();
            }
        });

        /**
         * test cancelGpuHighFreq
         * native function
         */
        ((Button) findViewById(R.id.cancel_gpu_high_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                final long requestId = HardCoderJNI.cancelGpuHighFreq(android.os.Process.myTid(),
                        SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "cancelGpuHighFreq, requestId:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "cancelGpuHighFreq, requestId:" + requestId,
                                Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

        /**
         * test requestCpuCoreForThread
         * native function
         * bind threads to cpu core
         */
        ((Button) findViewById(R.id.request_cpu_core_for_thread)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final long requestId = hcSwitch ? HardCoderJNI.requestCpuCoreForThread(SCENE_TEST, ACTION_TEST,
                                new int[]{android.os.Process.myTid()}, 6000, android.os.Process.myTid(),
                                SystemClock.elapsedRealtimeNanos()) : 0;
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        for(int i = 0; i < 1000; i++){
                            pitest();
                        }
                        HardCoderLog.i(TAG, "requestCpuCoreForThread, requestId:" + requestId + " take " +
                                (System.currentTimeMillis() - s) + "ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, "requestCpuCoreForThread, requestId:" + requestId,
                                        Toast.LENGTH_LONG).show();
                            }
                        });
                    }
                }).start();
            }
        });

        /**
         * test cancelCpuCoreForThread
         * native function
         * cancel bind threads to cpu core
         */
        ((Button) findViewById(R.id.cancel_cpu_core_for_thread)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                final long requestId = HardCoderJNI.cancelCpuCoreForThread(new int[]{android.os.Process.myTid()},
                        android.os.Process.myTid(), SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "cancelCpuCoreForThread, requestId:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "cancelCpuCoreForThread, requestId:" + requestId, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

        /**
         * test requestHighIOFreq
         * native function
         * bind threads to cpu core
         */
        ((Button) findViewById(R.id.request_high_IO_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                final long requestId = HardCoderJNI.requestHighIOFreq(SCENE_TEST, ACTION_TEST, HardCoderJNI.IO_LEVEL_1, 7,
                        android.os.Process.myTid(), SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "requestHighIOFreq, requestId:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "requestHighIOFreq, requestId:" + requestId, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

        //cancelHighIOFreq
        ((Button) findViewById(R.id.cancel_high_IO_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                final long requestId = HardCoderJNI.cancelHighIOFreq(android.os.Process.myTid(),
                        SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "cancelHighIOFreq:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "cancelHighIOFreq:" + requestId, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

        /**
         * open/close Hardcoder
         */
        hcSwitchBtn = ((Button) findViewById(R.id.hc_switch));
        hcSwitchBtn.setText(hcSwitch ? "Hardcoder state: opened" : "Hardcoder state: closed");
        hcSwitchBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                hcSwitch = hcSwitch ? false : true;
                if(hcSwitch) {
                    hcSwitchBtn.setText("Hardcoder state: opened");
                } else {
                    hcSwitchBtn.setText("Hardcoder state: closed");
                }
            }
        });

        /**
         * test startPerformance
         */
        ((Button) findViewById(R.id.start_performance)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final int[] threadId = new int[1];
                        threadId[0] = android.os.Process.myTid();
                        Thread t = new Thread(new Runnable() {
                            @Override
                            public void run() {
                                final int ret = hcSwitch ? HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
                                        HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
                                        new int[]{android.os.Process.myTid(), threadId[0]}, 6000, SCENE_TEST,
                                        ACTION_TEST, android.os.Process.myTid(), TAG) : 0;
                                lastHash = ret;
                                long s = System.currentTimeMillis();
                                //模拟重度计算场景，比较耗cpu操作
                                for (int i = 0; i < 50; i++){
                                    for (int j = 0; j < 10; j++){
                                        pitest();
                                    }
                                    try {
                                        Thread.sleep(50);
                                    } catch (InterruptedException e) {
                                        HardCoderLog.e(TAG, "startPerformance error:" + e.toString());
                                    }
                                }
                                HardCoderLog.i(TAG, "startPerformance, ret:" + ret + " take " +
                                        (System.currentTimeMillis() - s) + " ms");
                                fv.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, "startPerformance, ret:" + ret + ", thread id:" +
                                                android.os.Process.myTid() + ", " + threadId[0], Toast.LENGTH_LONG).
                                                show();
                                    }
                                });
                            }
                        });
                        t.setName("test_startPerformance");
                        t.start();

                        //模拟重度计算场景，比较耗cpu操作
                        for (int i = 0; i < 50; i++){
                            for (int j = 0; j < 50; j++){
                                pitest();
                            }
                            try {
                                Thread.sleep(50);
                            } catch (InterruptedException e) {
                                HardCoderLog.e(TAG, " error:" + e.toString());
                            }
                        }
                    }
                });
                s.setName("test_startThread");
                s.start();
            }
        });

        /**
         * test stopPerformance
         */
        ((Button) findViewById(R.id.stop_performace)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                HardCoderJNI.stopPerformance(lastHash);
            }
        });

        /**
         * test requestUnifyCpuIOThreadCoreGpu
         * repquest cpu, io, bindcores, gpu level at on time
         */
        ((Button) findViewById(R.id.requestUnify)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                final View fv = view;
                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final long requestId = hcSwitch ? HardCoderJNI.requestUnifyCpuIOThreadCoreGpu(SCENE_TEST, ACTION_TEST,
                                HardCoderJNI.CPU_LEVEL_1, HardCoderJNI.GPU_LEVEL_1, HardCoderJNI.IO_LEVEL_1,
                                new int[]{android.os.Process.myTid()}, 6000, android.os.Process.myTid(),
                                SystemClock.elapsedRealtimeNanos()) : 0;
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        for (int i = 0; i < 50; i++){
                            for (int j = 0; j < 10; j++){
                                pitest();
                            }
                            try {
                                Thread.sleep(50);
                            } catch (InterruptedException e) {
                                HardCoderLog.e(TAG, " error:" + e.toString());
                            }
                        }
                        final long time = System.currentTimeMillis() - s;
                        HardCoderLog.i(TAG, "requestUnifyCpuIOThreadCore:" + requestId + " take " + time + "ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, "requestUnifyCpuIOThreadCore:" + requestId + " time:%d" +
                                        time, Toast.LENGTH_LONG).show();
                            }
                        });
                    }

                });
                t.setName("test_requestUnifyCpuIOThreadCore");
                t.start();
            }
        });

        /**
         * test cancelUnifyCpuIOThreadCoreGpu
         */
        ((Button) findViewById(R.id.cancelUnify)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                HardCoderJNI.cancelUnifyCpuIOThreadCoreGpu(true, true, false, false, new int[]{111, 666},
                        android.os.Process.myTid(), SystemClock.elapsedRealtimeNanos());
            }
        });




        //跳转到第二测试页面
        ((Button) findViewById(R.id.multiProcess)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, SecondActivity.class);
                startActivity(intent);
            }
        });
    }

}

/**
 * calculate PI to test CPU
 */
class PITest {
    public static double test3() {
        double Pi = 4;
        boolean plus = false;
        for (int i = 3; i < 10000000; i += 2) {
            if (plus) {
                Pi += 4.0 / i;
            } else {
                Pi -= 4.0 / i;
            }
            plus = !plus;
        }
        return Pi;
    }

    public static double test2() {
        int i;
        int nThrows = 0;
        int nSuccess = 0;
        double x = 0;
        double y = 0;

        for (i = 0; i < 1000000; i++) {
            x = Math.random();
            y = Math.random();
            nThrows++;
            if (x * x + y * y <= 1) {
                nSuccess++;
            }
        }
        return 4 * (double) nSuccess / (double) nThrows;
    }


    private static int n = 100;//小数点后100位

    public static String pitest() {
        BigDecimal part1 = arctan(57).multiply(new BigDecimal(176));
        BigDecimal part2 = arctan(239).multiply(new BigDecimal(28));
        BigDecimal part3 = arctan(682).multiply(new BigDecimal(-48));
        BigDecimal part4 = arctan(12943).multiply(new BigDecimal(96));

        BigDecimal part = part1.add(part2).add(part3).add(part4);
        return part.toString();
    }

    public static BigDecimal arctan(int input) {
        int n2 = n + 2;//为了防误差

        BigDecimal result = BigDecimal.ZERO;
        BigDecimal xsquare = new BigDecimal(input * input);
        BigDecimal bigx = new BigDecimal(input);
        BigDecimal temp;
        BigDecimal res = BigDecimal.ONE.divide(bigx, n2, BigDecimal.ROUND_HALF_EVEN);

        boolean b = true;
        for (int i = 1; ; i += 2) {
            temp = res.divide(new BigDecimal(i), n2, BigDecimal.ROUND_HALF_EVEN);
            //根据莱布尼兹级数结果=0时返回
            if (temp.compareTo(BigDecimal.ZERO) == 0) {
                break;
            }
            if (b) {
                result = result.add(temp);//加
            } else {
                result = result.subtract(temp);//减
            }
            b = !b;
            res = res.divide(xsquare, n2, BigDecimal.ROUND_HALF_EVEN);
        }
        return result;
    }
}