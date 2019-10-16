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
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.tencent.mm.hardcoder.HardCoderCallback;
import com.tencent.mm.hardcoder.HardCoderJNI;
import com.tencent.mm.hardcoder.HardCoderLog;

import java.math.BigDecimal;

import static com.tencent.mm.hardcoder.testapp.PITest.test;

/**
 * Hardcoder testapp quick start
 * Test main APIs
 */
public class QuickStart extends Activity {

    private final static String TAG = "Hardcoder.QuickStart";

    /**
     * scene and action value for test
     */
    private final static int SCENE_TEST = 101;
    private final static long ACTION_TEST = 1 << 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.quick_start);

        /**
         * test initHardCoder
         */
        final TextView initHardcoderResultTextView = ((TextView) findViewById(R.id.init_hardCoder_result));
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
                                                initHardcoderResultTextView.setText(isConnect ? "Success" : "Failed, this device doesn't support Hardcoder.");
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
        final TextView checkPermissionResultTextView = ((TextView) findViewById(R.id.check_permission_result));
        ((Button) findViewById(R.id.check_permission)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                //manufacture为厂商
                //cert为testapp在OPPO手机上生成的key,具体值请向厂商申请生成
                //manufacture和cert均为数组，可同时传多个厂商的值
                String[] manufactures = {"OPPO"};
                String[] certs = {"K6dLGcYfPdHbdbnOwbXe7tXc3sySDV0IZoJ4u7CSB7yPpSJ2xb2lrHbHyIwrzNhuc88kuehvY" +
                        "MQhVsx6X8nOks0lfvTyNoOOYpX/3UinFnbSPvlXOt9U+v3sXhJPqbU7zR6fjghJ" +
                        "hfZYxSs+lOMgg1qwv0A0duXMEJWCzU+tnZ4="};
                final long requestId = HardCoderJNI.checkPermission(manufactures, certs,
                        new HardCoderCallback.FuncRetCallback() {
                        @Override
                        public void onFuncRet(int callbackType, long requestId, int retCode, int funcId, int dataType, byte[] buffer) {
                            final boolean isSuccess = (retCode == 0 ? true : false);
                            final boolean isNotSupport = (retCode == HardCoderJNI.ERR_UNAUTHORIZED ? true : false);
                            HardCoderLog.i(TAG, "checkpermission callback, retCode:" + retCode);
                            fv.post(new Runnable() {
                                @Override
                                public void run() {
                                    checkPermissionResultTextView.setText(isSuccess ? "Success" : (isNotSupport ? "This device not support checkPermission api, just ignore and continue." : "Failed, testapp doesn't get authentication to use Hardcoder at this device."));
                                }
                            });
                        }
                    });
                HardCoderLog.i(TAG, "checkPermission, requestId:" + requestId);
            }
        });

        HardCoderJNI.setHcDebug(true);


        /**
         * test startPerformance
         */
        final TextView startPerformanceHCTimeTextView = ((TextView) findViewById(R.id.start_performance_hc_time));
        ((Button) findViewById(R.id.start_performance_with_hc)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                startPerformanceHCTimeTextView.setText("waiting");
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final int[] threadId = new int[1];
                        threadId[0] = android.os.Process.myTid();
                        final int hashCode = HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
                                HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
                                new int[]{android.os.Process.myTid()}, 10000, SCENE_TEST,
                                ACTION_TEST, android.os.Process.myTid(), TAG);
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        test();
                        final long costTime = System.currentTimeMillis() - s;
                        HardCoderLog.i(TAG, "startPerformance HC, take " + costTime + " ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                startPerformanceHCTimeTextView.setText(costTime + "ms");
                            }
                        });
                        HardCoderJNI.stopPerformance(hashCode);
                    }
                });
                s.setName("testHC_startThread");
                s.start();
            }
        });

        /**
         * same test without Hardcoder
         */
        final TextView startPerformanceTimeTextView = ((TextView) findViewById(R.id.start_performance_time));
        ((Button) findViewById(R.id.start_performance)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                startPerformanceTimeTextView.setText("waiting");
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        test();
                        final long costTime = System.currentTimeMillis() - s;
                        HardCoderLog.i(TAG, "startPerformance, take " +
                                costTime/1000 + " s, " + costTime + " ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                startPerformanceTimeTextView.setText(Long.toString(costTime) + "ms");
                            }
                        });
                    }
                });
                s.setName("test_startThread");
                s.start();
            }
        });

        /**
         * jump to test more APIs
         */
        ((Button) findViewById(R.id.more_api)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Intent intent = new Intent();
                intent.setClass(QuickStart.this, TestAPIs.class);
                startActivity(intent);
            }
        });
    }

}

/**
 * calculate PI to test CPU
 */
class PITest {
    private final static String TAG = "hardcoder.PITest";
    public static void test(){
        //模拟重度计算场景，比较耗cpu操作
        for (int i = 0; i < 100; i++){
            for (int j = 0; j > -50; j--){
                pitest(i * j);
            }
            try {
                Thread.sleep(20);
            } catch (InterruptedException e) {
                HardCoderLog.e(TAG, "test error:" + e.toString());
            }
        }
    }

    private static int n = 100;//小数点后100位

    public static String pitest(int x) {
        BigDecimal part1 = arctan(57).multiply(new BigDecimal(176 + x));
        BigDecimal part2 = arctan(239).multiply(new BigDecimal(28 + x));
        BigDecimal part3 = arctan(682).multiply(new BigDecimal(-248 + x));
        BigDecimal part4 = arctan(8693).multiply(new BigDecimal(196 + x));
        BigDecimal part5 = arctan(12943).multiply(new BigDecimal(96 + x));
        BigDecimal part = part1.add(part2).add(part3).add(part4).add(part5);
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
}