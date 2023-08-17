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
import android.os.Handler;
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

    public final static int REQUEST_CODE_WEBVIEW_WITHOUT_HC = 1;
    public final static int REQUEST_CODE_WEBVIEW_WITH_HC = 2;
    public final static int REQUEST_CODE_WEBVIEW_AUTO_TEST = 3;
    public final static int REQUEST_CODE_WEBVIEW_HC_AUTO_TEST = 4;
    public final static int REQUEST_CODE_VIDEO_WITHOUT_HC = 5;
    public final static int REQUEST_CODE_VIDEO_WITH_HC = 6;
    public final static int REQUEST_CODE_VIDEO_AUTO_TEST = 7;
    public final static int REQUEST_CODE_VIDEO_HC_AUTO_TEST = 8;

    public static long testWebViewStartTime = 0L;
    public static long testHCWebViewStartTime = 0L;

    public static long testVideoStartTime = 0L;
    public static long testHCVideoStartTime = 0L;


    private TextView openWebViewTimeTextView, hcOpenWebViewTimeTextView, autoTestOpenWebViewTimeTextView;
    private TextView openVideoTimeTextView, hcOpenVideoTimeTextView, autoTestOpenVideoTimeTextView;

    private final static int TEST_COUNT = 10;
    private static int testTime = 0;


    private static long totalCostTime = 0L;
    private static long totalHCCostTime = 0L;

    private static long[] costTimeArray, hcCostTimeArray;

    private static StringBuilder resultString;

    private static int lastWebviewHashCode, lastVideoHashCode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.quick_start);
        HardCoderJNI.setHcDebug(true);

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
                                        //manufacture为厂商，cert为testapp在OPPO手机上生成的key,具体值请向厂商申请生成
                                        //manufacture和cert均为数组，可同时传多个厂商的值
                                        String[] manufactures = {"OPPO"};
                                        String[] certs = {"K6dLGcYfPdHbdbnOwbXe7tXc3sySDV0IZoJ4u7CSB7yPpSJ2xb2lrHbHyIwrzNhuc88kuehvY" +
                                                "MQhVsx6X8nOks0lfvTyNoOOYpX/3UinFnbSPvlXOt9U+v3sXhJPqbU7zR6fjghJ" +
                                                "hfZYxSs+lOMgg1qwv0A0duXMEJWCzU+tnZ4="};
                                        final long requestId = HardCoderJNI.checkPermission(manufactures, certs,
                                                new HardCoderCallback.FuncRetCallback() {
                                                    @Override
                                                    public void onFuncRet(int callbackType, long requestId, int retCode, int funcId, int dataType, byte[] buffer) {
                                                        HardCoderLog.i(TAG, "checkpermission callback, retCode:" + retCode);
                                                    }
                                                });
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


        openWebViewTimeTextView = ((TextView) findViewById(R.id.open_webview_time));
        hcOpenWebViewTimeTextView = ((TextView) findViewById(R.id.HC_open_webview_time));
        autoTestOpenWebViewTimeTextView = ((TextView) findViewById(R.id.open_webview_auto_test_time));


        ((Button) findViewById(R.id.test_open_webview)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                openWebViewTimeTextView.setText("waiting");
                testWebView(REQUEST_CODE_WEBVIEW_WITHOUT_HC, false);
            }
        });

        ((Button) findViewById(R.id.test_HC_open_webview)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                hcOpenWebViewTimeTextView.setText("waiting");
                testWebViewHC(REQUEST_CODE_WEBVIEW_WITH_HC, false);
            }
        });



        ((Button) findViewById(R.id.auto_test_open_webview)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                testTime = 0;
                costTimeArray = new long[TEST_COUNT];
                hcCostTimeArray = new long[TEST_COUNT];
//                totalCostTime = 0L;
//                totalHCCostTime = 0L;
                autoTestOpenWebViewTimeTextView.setText("Waiting");
                resultString = new StringBuilder();
                testWebView(REQUEST_CODE_WEBVIEW_AUTO_TEST, true);
            }
        });


//        openVideoTimeTextView = ((TextView) findViewById(R.id.open_video_time));
//        hcOpenVideoTimeTextView = ((TextView) findViewById(R.id.HC_open_video_time));
//        autoTestOpenVideoTimeTextView = ((TextView) findViewById(R.id.open_video_auto_test_time));
//
//        ((Button) findViewById(R.id.test_open_video)).setOnClickListener(new View.OnClickListener() {
//            public void onClick(View view) {
//                testVideo(REQUEST_CODE_VIDEO_WITHOUT_HC, false);
//            }
//        });
//
//        ((Button) findViewById(R.id.test_HC_open_video)).setOnClickListener(new View.OnClickListener() {
//            public void onClick(View view) {
//                testVideoHC(REQUEST_CODE_VIDEO_WITH_HC, false);
//            }
//        });
//        ((Button) findViewById(R.id.auto_test_open_video)).setOnClickListener(new View.OnClickListener() {
//            public void onClick(View view) {
//                testTime = 0;
//                totalCostTime = 0L;
//                totalHCCostTime = 0L;
//                autoTestOpenVideoTimeTextView.setText("Waiting");
//                testVideo(REQUEST_CODE_VIDEO_AUTO_TEST, true);
//            }
//        });


        final TextView testArithmeticTimeTextView = ((TextView) findViewById(R.id.test_arithmetic_time));
        final TextView testArithmeticHCTimeTextView = ((TextView) findViewById(R.id.test_arithmetic_hc_time));
        final TextView autoTestArithmeticTimeTextView = ((TextView) findViewById(R.id.arithmetic_auto_test_time));


        ((Button) findViewById(R.id.test_arithmetic)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                testArithmeticTimeTextView.setText("waiting");
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {//delay 一下等cpu稳定下来再执行
                            Thread.sleep(3000);
                        } catch (InterruptedException e) {
                        }
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        test();
                        final long costTime = System.currentTimeMillis() - s;
                        HardCoderLog.i(TAG, "startPerformance, take " +
                                costTime/1000 + " s, " + costTime + " ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                testArithmeticTimeTextView.setText(costTime + "ms");
                            }
                        });
                    }
                });
                s.setName("test_startThread");
                s.start();
            }
        });

        ((Button) findViewById(R.id.test_hc_arithmetic)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;

                testArithmeticHCTimeTextView.setText("waiting");
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final int[] threadId = new int[1];
                        threadId[0] = android.os.Process.myTid();
                        final int hashCode = HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
                                HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
                                new int[]{android.os.Process.myTid()}, 15000, SCENE_TEST,
                                ACTION_TEST, android.os.Process.myTid(), TAG);
                        try {//Hardcoder请求异步，delay 1s方便测试
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                        }
                        long s = System.currentTimeMillis();
                        //模拟重度计算场景，比较耗cpu操作
                        test();
                        final long costTime = System.currentTimeMillis() - s;
                        HardCoderLog.i(TAG, "startPerformance HC, take " + costTime + " ms");
                        fv.post(new Runnable() {
                            @Override
                            public void run() {
                                testArithmeticHCTimeTextView.setText(costTime + "ms");
                            }
                        });
                        HardCoderJNI.stopPerformance(hashCode);
                    }
                });
                s.setName("testHC_startThread");
                s.start();

            }
        });

        ((Button) findViewById(R.id.auto_test_arithmetic)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                final View fv = view;
                testTime = 0;
                totalCostTime = 0L;
                totalHCCostTime = 0L;
                resultString = new StringBuilder("costTime:\n");
                autoTestArithmeticTimeTextView.setText("Waiting");
                Thread s = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        for(int i = 0; i < TEST_COUNT; i++){
                            try {//等cpu稳定
                                Thread.sleep(3000);
                            } catch (InterruptedException e) {
                            }
                            long s = System.currentTimeMillis();
                            //模拟重度计算场景，比较耗cpu操作
                            test();
                            final long costTime = System.currentTimeMillis() - s;
                            totalCostTime += costTime;
                            resultString.append((i + 1) + ". " + costTime + "ms,   ");
                            fv.post(new Runnable() {
                                @Override
                                public void run() {
                                    autoTestArithmeticTimeTextView.setText(resultString.toString());
                                }
                            });
                        }

                        resultString.append("\nHC costTime:\n");

                        try {
                            Thread.sleep(5000);
                        } catch (InterruptedException e) {
                        }

                        for(int i = 0; i < TEST_COUNT; i++){
                            final int[] threadId = new int[1];
                            threadId[0] = android.os.Process.myTid();
                            final int hashCode = HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
                                    HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
                                    new int[]{android.os.Process.myTid()}, 15000, SCENE_TEST,
                                    ACTION_TEST, android.os.Process.myTid(), TAG);
                            try {//Hardcoder请求异步，delay 1s方便测试
                                Thread.sleep(1000);
                            } catch (InterruptedException e) {
                            }
                            long s = System.currentTimeMillis();
                            //模拟重度计算场景，比较耗cpu操作
                            test();
                            final long costTime = System.currentTimeMillis() - s;
                            totalHCCostTime += costTime;
                            resultString.append((i + 1) + ". " + costTime + "ms,   ");
                            fv.post(new Runnable() {
                                @Override
                                public void run() {
                                    autoTestArithmeticTimeTextView.setText(resultString.toString());
                                }
                            });
                            HardCoderJNI.stopPerformance(hashCode);
                        }

                        if(TEST_COUNT != 0){
                            resultString.append("\nFinish, average costTime:" + totalCostTime/TEST_COUNT +
                                    "ms, HC average costTime:" + totalHCCostTime/TEST_COUNT + "ms.");
                            fv.post(new Runnable() {
                                @Override
                                public void run() {
                                    autoTestArithmeticTimeTextView.setText(resultString.toString());
                                }
                            });
                        }
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


    private void testWebView(int requestCode, boolean isAuto){
        try {//delay 一下等cpu稳定下来再执行
            Thread.sleep(3000);
        } catch (InterruptedException e) {
        }
        testWebViewStartTime = System.currentTimeMillis();
        Intent intent = new Intent();
        intent.setClass(QuickStart.this, TestWebView.class);
        Bundle bundle = new Bundle();
        bundle.putBoolean("isAuto", isAuto);
        intent.putExtras(bundle);
        startActivityForResult(intent, requestCode);
    }

    private void testWebViewHC(int requestCode, boolean isAuto){
        //在新的进程打开webview，这里只使用了提频，传空int数组表示不使用绑核
        lastWebviewHashCode = HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
                HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
                new int[]{}, 5000, SCENE_TEST,
                ACTION_TEST, android.os.Process.myTid(), TAG);
        try {//Hardcoder请求异步，delay 1s方便测试
            Thread.sleep(1000);
        } catch (InterruptedException e) {
        }
        testHCWebViewStartTime = System.currentTimeMillis();
        Intent intent = new Intent();
        intent.setClass(QuickStart.this, TestWebView.class);
        Bundle bundle = new Bundle();
        bundle.putBoolean("isAuto", isAuto);
        intent.putExtras(bundle);
        startActivityForResult(intent, requestCode);
    }

//    private void testVideo(int requestCode, boolean isAuto){
//        testVideoStartTime = System.currentTimeMillis();
//        Intent intent = new Intent();
//        intent.setClass(QuickStart.this, TestVideo.class);
//        Bundle bundle = new Bundle();
//        bundle.putBoolean("isAuto", isAuto);
//        intent.putExtras(bundle);
//        startActivityForResult(intent, requestCode);
//    }
//
//    private void testVideoHC(int requestCode, boolean isAuto){
//        lastVideoHashCode = HardCoderJNI.startPerformance(0, HardCoderJNI.CPU_LEVEL_1,
//                HardCoderJNI.IO_LEVEL_1, HardCoderJNI.GPU_LEVEL_1,
//                new int[]{}, 3000, SCENE_TEST,
//                ACTION_TEST, android.os.Process.myTid(), TAG);
//        try {//Hardcoder请求异步，delay 1s方便测试
//            Thread.sleep(1000);
//        } catch (InterruptedException e) {
//        }
//        testHCVideoStartTime = System.currentTimeMillis();
//        Intent intent = new Intent();
//        intent.setClass(QuickStart.this, TestVideo.class);
//        Bundle bundle = new Bundle();
//        bundle.putBoolean("isAuto", isAuto);
//        intent.putExtras(bundle);
//        startActivityForResult(intent, requestCode);
//    }

    private void getAveWithoutUnusualVallue(long[] results, long[] hcResults, int testTime){
        long totalCostTime = 0L;
        long hcTotalCostTime = 0L;
        for(int i = 0; i < testTime; i++){
            totalCostTime += results[i];
            hcTotalCostTime += hcResults[i];
        }
        long ave = totalCostTime / testTime;
        long hcAve = hcTotalCostTime / testTime;


        int realTestTime = testTime;
        StringBuilder deleteResult = new StringBuilder();
        for(int i = 0; i < testTime; i++){
            if(results[i] > 1.3 * ave || hcResults[i] > 1.3 * hcAve) {//大于平均值1.3倍视为当次测试网络有异常，去掉测试值
                deleteResult.append((i + 1) + ", ");
                totalCostTime -= results[i];
                hcTotalCostTime -= hcResults[i];
                realTestTime --;
            }
        }
        if(realTestTime < testTime){//重新计算平均值
            ave = totalCostTime / realTestTime;
            hcAve = hcTotalCostTime / realTestTime;
        }
        resultString.append("Finish, delete unusual results:" + (deleteResult.toString().equals("") ? "null, " : deleteResult.toString()) +
                " average costTime:" + ave + "ms, HC average costTime:" + hcAve + "ms.");
    }

    @Override
    public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch(requestCode){
            case REQUEST_CODE_WEBVIEW_WITHOUT_HC:
                if(data != null) {
                    long costTime = data.getLongExtra("endTime", 0L) - testWebViewStartTime;
                    testWebViewStartTime = 0L;
                    openWebViewTimeTextView.setText(costTime + "ms");
                }
                break;
            case REQUEST_CODE_WEBVIEW_WITH_HC:
                if(data != null) {
                    long hcCostTime = data.getLongExtra("endTime", 0L) - testHCWebViewStartTime;
                    testHCWebViewStartTime = 0L;
                    hcOpenWebViewTimeTextView.setText(hcCostTime + "ms");
//                    if(lastWebviewHashCode != 0){
//                        HardCoderJNI.stopPerformance(lastWebviewHashCode);
//                    }
                }
                break;

            case REQUEST_CODE_WEBVIEW_AUTO_TEST:
                if(data != null) {
                    long costTime = data.getLongExtra("endTime", 0L) - testWebViewStartTime;
                    testWebViewStartTime = 0L;
                    costTimeArray[testTime] = costTime;
                    resultString.append((testTime + 1) + ". costTime:" + costTime);
                    autoTestOpenWebViewTimeTextView.setText(resultString.toString());
                    new Handler().postDelayed(new Runnable(){
                        public void run() {
                            testWebViewHC(REQUEST_CODE_WEBVIEW_HC_AUTO_TEST, true);
                        }
                    }, 3000);
                }
                break;
            case REQUEST_CODE_WEBVIEW_HC_AUTO_TEST:
                if(data != null) {
                    long hcCostTime = data.getLongExtra("endTime", 0L) - testHCWebViewStartTime;
                    testHCWebViewStartTime = 0L;
                    hcCostTimeArray[testTime] = hcCostTime;
//                    if(lastWebviewHashCode != 0){
//                        HardCoderJNI.stopPerformance(lastWebviewHashCode);
//                    }
                    testTime ++;
                    resultString.append(", HC costTime:" + hcCostTime + "\n");
                    autoTestOpenWebViewTimeTextView.setText(resultString.toString());
                    if (testTime >= TEST_COUNT) {
                        if(testTime == 0){
                            autoTestOpenWebViewTimeTextView.setText("Error!");
                        }
                        getAveWithoutUnusualVallue(costTimeArray, hcCostTimeArray, testTime);
                        autoTestOpenWebViewTimeTextView.setText(resultString.toString());
                    } else {
                        new Handler().postDelayed(new Runnable(){
                            public void run() {
                                testWebView(REQUEST_CODE_WEBVIEW_AUTO_TEST, true);
                            }
                        }, 6000);
                    }
                }
                break;
//            case REQUEST_CODE_VIDEO_WITHOUT_HC:
//                if(data != null) {
//                    long costTime = data.getLongExtra("endTime", 0L) - testVideoStartTime;
//                    testVideoStartTime = 0L;
//                    openVideoTimeTextView.setText(costTime + "ms");
//                }
//                break;
//            case REQUEST_CODE_VIDEO_WITH_HC:
//                if(data != null) {
//                    long hcCostTime = data.getLongExtra("endTime", 0L) - testHCVideoStartTime;
//                    testHCVideoStartTime = 0L;
//                    hcOpenVideoTimeTextView.setText(hcCostTime + "ms");
////                    if(lastVideoHashCode != 0){
////                        HardCoderJNI.stopPerformance(lastVideoHashCode);
////                    }
//                }
//                break;
//
//            case REQUEST_CODE_VIDEO_AUTO_TEST:
//                if(data != null) {
//                    long costTime = data.getLongExtra("endTime", 0L) - testVideoStartTime;
//                    testVideoStartTime = 0L;
//                    openVideoTimeTextView.setText(costTime + "ms");
//                    totalCostTime += costTime;
//                    new Handler().postDelayed(new Runnable(){
//                        public void run() {
//                            testVideoHC(REQUEST_CODE_VIDEO_HC_AUTO_TEST, true);
//                        }
//                    }, 3000);
//                }
//                break;
//            case REQUEST_CODE_VIDEO_HC_AUTO_TEST:
//                if(data != null) {
//                    long hcCostTime = data.getLongExtra("endTime", 0L) - testHCVideoStartTime;
//                    testHCVideoStartTime = 0L;
//                    hcOpenVideoTimeTextView.setText(hcCostTime + "ms");
//                    totalHCCostTime += hcCostTime;
////                    if(lastVideoHashCode != 0){
////                        HardCoderJNI.stopPerformance(lastVideoHashCode);
////                    }
//                    testTime ++;
//                    if (testTime >= TEST_COUNT) {
//                        if(testTime == 0){
//                            autoTestOpenVideoTimeTextView.setText("Error!");
//                        }
//                        autoTestOpenVideoTimeTextView.setText("Finish, average costTime:" + totalCostTime/testTime + "ms, HC average costTime:" + totalHCCostTime/testTime + "ms");
//                    } else {
//                        new Handler().postDelayed(new Runnable(){
//                            public void run() {
//                                testVideo(REQUEST_CODE_VIDEO_AUTO_TEST, true);
//                            }
//                        }, 6000);
//                    }
//                }
//                break;
            default:
                break;
        }
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