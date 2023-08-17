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
import android.view.KeyEvent;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

/**
 * test open webview
 */
public class TestWebView extends Activity {

    private final static String TAG = "Hardcoder.TestWebView";

    public static long endTime = 0L;
    public final static int RESULT_CODE = 0;
    private static boolean isAuto = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_webview);
        isAuto = getIntent().getBooleanExtra("isAuto", false);

        final WebView webview = (WebView) findViewById(R.id.open_webview);
        WebSettings webSettings = webview.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setAllowFileAccess(true);
        webSettings.setBuiltInZoomControls(true);
        webSettings.setCacheMode(WebSettings.LOAD_NO_CACHE);

        webview.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                endTime = System.currentTimeMillis();
                Toast.makeText(TestWebView.this, "Webview load finish, please goBack.", Toast.LENGTH_LONG).show();
                if(isAuto){
                    new Handler().postDelayed(new Runnable(){
                        public void run() {
                            exitWebview();
                        }
                    }, 2000);
                }
            }
        });
        webview.loadUrl("https://github.com/Tencent/Hardcoder");
    }

    @Override
    public void onDestroy() {
        exitWebview();
        super.onDestroy();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
            exitWebview();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    private void exitWebview(){
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        bundle.putLong("endTime", endTime);
        setResult(RESULT_CODE, intent.putExtras(bundle));
        finish();
        android.os.Process.killProcess(android.os.Process.myPid());
    }
}

