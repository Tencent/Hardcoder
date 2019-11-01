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
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * test open video
 */
public class TestVideo extends Activity {

    private final static String TAG = "Hardcoder.TestVideo";

    public static long endTime = 0L;
    public final static int RESULT_CODE = 0;

    private MediaPlayer mediaPlayer;

    String outPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_video);

        SurfaceView videoSuf = (SurfaceView) findViewById(R.id.open_video);
        videoSuf.setZOrderOnTop(true);
        videoSuf.getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        videoSuf.getHolder().addCallback(surfaceCallback);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mediaPlayer.isPlaying()) {
            mediaPlayer.stop();
        }
    }

    private SurfaceHolder.Callback surfaceCallback = new SurfaceHolder.Callback(){
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            mediaPlayer = new MediaPlayer();
            outPath = Environment.getExternalStorageDirectory().getPath() + "/hardcoder_video.mp4";
            try {
                if(!new File(outPath).getParentFile().exists()){
                    new File(outPath).getParentFile().mkdir();
                }
                copyStream(TestVideo.this.getResources().openRawResource(R.raw.hardcoder_video), new FileOutputStream(outPath));
                mediaPlayer.setDataSource(outPath);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }

            mediaPlayer.setDisplay(surfaceHolder);
            mediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener(){
                @Override
                public void onPrepared(MediaPlayer mediaPlayer) {
                    mediaPlayer.start();
                    endTime = System.currentTimeMillis();
                    Toast.makeText(TestVideo.this, "Video load finish, please goBack.", Toast.LENGTH_LONG).show();
                }
            });
            mediaPlayer.prepareAsync();
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

        }
    };

    public static long copyStream(InputStream from, OutputStream to) {
        if (from == null || to == null) {
            return 0;
        }
        try {
            byte[] buf = new byte[0x1000];
            long total = 0;
            while (true) {
                int r = from.read(buf);
                if (r == -1) {
                    break;
                }
                to.write(buf, 0, r);
                total += r;
            }
            return total;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        if (null != mediaPlayer) {
            mediaPlayer.release();
            mediaPlayer = null;
        }
        exitVideo();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
            exitVideo();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    private void exitVideo(){
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        bundle.putLong("endTime", endTime);
        setResult(RESULT_CODE, intent.putExtras(bundle));
        if(outPath != null) {
            new File(outPath).delete();
        }
        finish();
        android.os.Process.killProcess(android.os.Process.myPid());
    }
}
