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

import android.util.Log;


/**
 * If app wants to use customized Log, use setLog.
 * Otherwise just use the system default Log.
 */
public class HardCoderLog {
    private static ILog iLog = null;
    public interface ILog{
        void i(String tag, String msg);
        void d(String tag, String msg);
        void e(String tag, String msg);
        void printErrStackTrace(String tag, Throwable tr, String format, Object... obj);
    }

    public static void setLog(ILog log){
        iLog = log;
    }

    public static void i(final String tag, final String msg) {
        if(iLog == null) {
            Log.i(tag, msg);
        } else {
            iLog.i(tag, msg);
        }
    }

    public static void d(final String tag, final String msg) {
        if (HardCoderJNI.isHcDebug()) {//if only open debug log
            if (iLog == null) {
                Log.d(tag, msg);
            } else {
                iLog.d(tag, msg);
            }
        }
    }

    public static void e(final String tag, final String msg) {
        if(iLog == null) {
            Log.e(tag, msg);
        } else {
            iLog.e(tag, msg);
        }
    }

    public static void printErrStackTrace(String tag, Throwable throwable, final String format, final Object... obj) {
        if(iLog == null) {
            throwable.printStackTrace();
        } else {
            iLog.printErrStackTrace(tag, throwable, format, obj);
        }
    }
}
