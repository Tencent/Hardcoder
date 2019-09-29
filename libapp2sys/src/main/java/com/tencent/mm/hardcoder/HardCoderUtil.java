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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Vector;

/**
 * utils
 */
public final class HardCoderUtil {
    private final static String TAG = "Hardcoder.HardCoderUtil";

    private HardCoderUtil() {
    }

    /**
     * get process cpu time
     * @return cpu time
     */
    public static long[] getMyProcCpuTime() {
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;
        try {
            fileReader = new FileReader("/proc/" + Process.myPid() + "/stat");
            bufferedReader = new BufferedReader(fileReader);
            String stat = bufferedReader.readLine();
            String[] arr = stat.split(" ");
            return new long[]{Long.parseLong(arr[13]), Long.parseLong(arr[14])};
        } catch (Exception e) {
            HardCoderLog.e(TAG, "getMyProcCpuTime exception:" + e.getMessage());
        } finally {
            if (fileReader != null) {
                try {
                    fileReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getMyProcCpuTime fileReader close exception:" +
                            e.getMessage());
                }
            }
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getMyProcCpuTime bufferedReader close exception:" +
                            e.getMessage());
                }
            }
        }
        return null;
    }

    /**
     * identify in which core this thread is running
     * @param linuxTid thread id
     * @return core id
     */
    public static int getThreadCoreId(int linuxTid) {
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;
        try {
            fileReader = new FileReader("/proc/" + Process.myPid() + "/task/" + linuxTid + "/stat");
            bufferedReader = new BufferedReader(fileReader);
            String stat = bufferedReader.readLine();
            if (stat == null) {
                return HardCoderJNI.ERROR_CODE_FAILED;
            }
            String[] arr = stat.split(" ");
            return HardCoderUtil.getInt(arr[38], 0);
        } catch (Exception e) {
            HardCoderLog.e(TAG, "getThreadCoreId exception:" + e.getMessage());
        } finally {
            if (fileReader != null) {
                try {
                    fileReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getThreadCoreId fileReader close exception:" +
                            e.getMessage());
                }
            }
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getThreadCoreId bufferedReader close exception:" +
                            e.getMessage());
                }
            }
        }
        return HardCoderJNI.ERROR_CODE_FAILED;
    }

    /**
     * get the cpu frequency of the core
     * @param coreId core id
     * @return cpu frequency
     */
    public static long getCpuFreqByCoreId(int coreId) {
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;
        try {
            File file = new File("/sys/devices/system/cpu/cpu" + coreId +
                    "/cpufreq/scaling_cur_freq");
            if (!file.exists()) {
                return HardCoderJNI.ERROR_CODE_FAILED;
            }
            fileReader = new FileReader(file);
            bufferedReader = new BufferedReader(fileReader);
            String line = bufferedReader.readLine();
            return HardCoderUtil.getLong(line, 0);
        } catch (Exception e) {
            HardCoderLog.e(TAG, "getCpuFreqByCoreId exception:" + e.getMessage());
        } finally {
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getCpuFreqByCoreId bufferedReader close exception:" +
                            e.getMessage());
                }
            }

            if (fileReader != null) {
                try {
                    fileReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getCpuFreqByCoreId fileReader close exception:" +
                            e.getMessage());
                }
            }
        }
        return HardCoderJNI.ERROR_CODE_FAILED;
    }

    /**
     * get the cpu frequency of the current core
     * @return cpu frequency
     */
    public static long[] getCurrCpuFreq() {
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;
        try {
            Vector<Long> vec = new Vector<>();
            for (int i = 0; i < 32; i++) {

                File file = new File("/sys/devices/system/cpu/cpu" + i + "/cpufreq/scaling_cur_freq");
                if (!file.exists()) {
                    break;
                }
                fileReader = new FileReader(file);
                bufferedReader = new BufferedReader(fileReader);
                String line = bufferedReader.readLine();
                bufferedReader.close();
                vec.add(HardCoderUtil.getLong(line, 0));
            }
            if (vec.size() <= 0) {
                return null;
            }
            long[] freqs = new long[vec.size()];
            for (int i = 0; i < vec.size(); i++) {
                freqs[i] = vec.get(i);
            }
            return freqs;
        } catch (Exception e) {
            HardCoderLog.e(TAG, "getCurrCpuFreq exception:" + e.getMessage());
        } finally {
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getCurrCpuFreq bufferedReader close exception:" +
                            e.getMessage());
                }
            }

            if (fileReader != null) {
                try {
                    fileReader.close();
                } catch (Exception e) {
                    HardCoderLog.e(TAG, "getCurrCpuFreq fileReader close exception:" +
                            e.getMessage());
                }
            }
        }
        return null;
    }

    public static int getInt(final String string, final int def) {
        try {
            return (string == null || string.length() <= 0) ? def : Integer.decode(string);
        } catch (NumberFormatException e) {
            HardCoderLog.printErrStackTrace(TAG, e, "getInt");
        }
        return def;
    }

    public static long getLong(final String string, final long def) {
        try {
            return (string == null || string.length() <= 0) ? def : Long.decode(string);
        } catch (NumberFormatException e) {
            HardCoderLog.printErrStackTrace(TAG, e, "getLong");
        }
        return def;
    }
}
