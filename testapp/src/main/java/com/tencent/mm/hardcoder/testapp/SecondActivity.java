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
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.tencent.mm.hardcoder.HardCoderCallback;
import com.tencent.mm.hardcoder.HardCoderJNI;
import com.tencent.mm.hardcoder.HardCoderLog;

/**
 * test activity, test multi process
 * SecondActivity runs at sub_process process, see Manifest
 */

public class SecondActivity extends Activity {
    private static final String TAG = "Hardcoder.SecondActivity";

    //测试scene值
    private static final int SCENE_TEST = 201;
    //测试action值
    private static final long ACTION_TEST = 1 << 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.second_activity);
        System.loadLibrary("hardcoder");

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
                        final int ret = HardCoderJNI.initHardCoder(remote, 0, HardCoderJNI.CLIENT_SOCK, new HardCoderJNI.HCPerfManagerThread(){
                            @Override
                            public Thread newThread(Runnable r, String name, int priority) {
                                return new Thread(r, name);
                            }
                        }, new HardCoderCallback.ConnectStatusCallback() {
                            @Override
                            public void onConnectStatus(final boolean isConnect) {
                                HardCoderLog.i(TAG, "initHardCoder callback, isConnectSuccess:" + isConnect);
                                fv.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(SecondActivity.this, "initHardCoder callback, server socket name:" +
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
         * test requestCpuHighFreq
         */
        ((Button) findViewById(R.id.request_cpu_high_freq)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final View fv = v;
                final long requestId = HardCoderJNI.requestCpuHighFreq(SCENE_TEST, ACTION_TEST, HardCoderJNI.CPU_LEVEL_1,
                        10000, android.os.Process.myTid(), SystemClock.elapsedRealtimeNanos());
                HardCoderLog.i(TAG, "requestCpuHighFreq, requestId:" + requestId);
                fv.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(SecondActivity.this, "requestCpuHighFreq, requestId:" + requestId, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });
    }
}
