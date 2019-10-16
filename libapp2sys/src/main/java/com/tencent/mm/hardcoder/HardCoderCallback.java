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

public class HardCoderCallback {

    /**
     * callback for localsocket connect status
     */
    public interface ConnectStatusCallback {
        void onConnectStatus(boolean isConnect);
    }

    /**
     * callback for function id request
     */
    public interface FuncRetCallback {
        void onFuncRet(final int callbackType, final long requestId, final int retCode,
                       final int funcId, final int dataType, final byte[] buffer);
    }

    /**
     * callback for scene value
     */
    public interface SceneReportCallback{
        void sceneReport(int scene, long action);
    }

    /**
     * RequestStatus mark the request info when use startPerformance
     */
    public static class RequestStatus {
        public int scene;
        public long action;
        public int cpulevel, gpulevel, iolevel;
        public int[] bindtids;

        public RequestStatus(int scene, long action, int cpulevel,
                             int gpulevel, int iolevel, int[] bindtids) {
            this.scene = scene;
            this.action = action;
            this.cpulevel = cpulevel;
            this.gpulevel = gpulevel;
            this.iolevel = iolevel;
            this.bindtids = bindtids.clone();
        }

        @Override
        public String toString() {
            StringBuilder stringBuilder = new StringBuilder("[RequestStatus, ")
                    .append("scene:").append(scene)
                    .append(", action:").append(action)
                    .append(", cpulevel:").append(cpulevel)
                    .append(", gpulevel:").append(gpulevel)
                    .append(", iolevel:").append(iolevel)
                    .append(", bindtids size:").append(bindtids.length);
            return stringBuilder.append("]").toString();
        }
    }
}
