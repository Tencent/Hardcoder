# Hardcoder 接入指南



通过 testapp 测试了 Hardcoder 主要接口和效果后（测试方法请见 [Hardcoder Testapp 测试指南](./hardcoder_testapp_test_guide.md)），接下来开发者可以把已有 APP 接入 Hardcoder。



### 接入步骤

1. 下载 Hardcoder 工程编译 aar；
2. 项目 `build.gradle ` 引入 Hardcoder aar； 
3. 进程启动时调用 initHardCoder 建立 socket 连接（一般进程启动时需要请求资源，因而推荐在进程启动时调用）。每个进程都是独立的，都需要调用 initHardCoder 建立 socket 连接，建立连接后每个进程维持一个 socket，进程退出时 socket 也会断开；
4. initHardCoder 回调成功后调用 checkPermission，传入 APP 已申请的各个厂商鉴权值；
5. 在需要请求资源的场景调用 startPerformance，传入请求资源的参数。若场景位于进程启动阶段，比如 APP 启动，需要在 initHardCoder 的回调成功以后再调用 startPerformance，确保连接已成功建立，或者判断 HardCoderJNI 的 isConnect() 检查 socket 是否已连接。
6. 场景结束时主动调用 stopPerformance，传入对应场景 startPerformance 时的返回值 hashCode 作为参数，停止本次请求。
7. 测试性能，APP 可对打开/关闭 Hardcoder 的情况做对比实验，测试性能数据。



### 编译 Hardcoder aar

项目 IDE 为 AndroidStudio，采用 gradle 构建，其中 Native 部分代码使用 CMAKE 编译。

1. 根目录下运行命令行 `gradlew assembleDebug` 触发子工程  libapp2sys 编译，编译成功后生成 Hardcoder aar， 输出目录为`hardcoder/libapp2sys/build/outputs/aar/libapp2sys-debug.aar`；
2. 若需要把 aar publish 到本地 maven 库，以便自身项目使用，在根目录下运行命令行 .`/gradlew publishToMavenLocal`，输出 aar 目录为（以Mac为例）： User 目录/.m2/repository/com/tencent/mm/hardcoder/app2sys/。

编译可能遇到的问题见 [Hardcoder Testapp 测试指南](./hardcoder_testapp_test_guide.md)。



### APP 引入 Hardcoder aar

Hardcoder 工程以 aar 方式引入，把 hardcoder:app2sys 添加到主工程目录下的 `build.gradle` 文件的依赖库中：

```gradle
dependencies {
    api('com.tencent.mm.hardcoder:app2sys:1.0.0')
}
```

其中 1.0.0 为当前 Hardcoder 版本号，当前版本号在 Hardcoder 主工程目录下 `gradle.properties` 文件。

```
HC_VERSION_NAME=1.0.0
```



### 调用 Hardcoder 方法

`HardCoderJNI.java` 中提供了 APP 需要调用的接口。

带 native 关键字函数为 jni 接口，为 Hardcoder client 端与 server 端约定的所有通信接口。一般不建议直接使用 JNI 接口，HardCoderJNI.java 提供了封装的 JAVA 层接口。



##### 初始化 initHardCoder

InitHardCoder 负责建立 socket 连接。调用 initHardCoder 前，请确保已正确设置了 hcEnable （上层接口是否开启 Hardcoder 功能开关，默认为 true）和 hcDebug （Hardcoder 组件是否打印 debug log，默认为 false）标志位。

所有与系统间通信都需要依赖 socket 连接，所以调用请求前确保已调用 initHardCoder 接口 。

```
public static int initHardCoder(String remote, int port, String local, HCPerfManagerThread hcPerfManagerThread, HardCoderCallback.ConnectStatusCallback callback)
```

remote 为 Hardcoder server 端 socket name，默认调用 HardCoderJNI.readServerAddr() 读取；

port  为 Hardcoder server 端端口值，可传入任意 int 值；

local 为 Hardcoder client 端 socket name，主要用于标识 client 端，可传入任意非空字符串；

hcPerfManagerThread 为运行 HCPerfManager 的线程，可传入 null，默认由 Hardcoder 新建一个线程运行；若使用 APP 已有线程池中线程，请传入接口 HardCoderJNI.HCPerfManagerThread 的实现类；

callback 为 initHardCoder 回调，获取连接建立成功/失败回调，不需要回调时可传入 null。建议在回调成功建立 socket 后再发起请求，否则可能存在请求时 socket 未成功连接导致请求失败的情况。  



InitHardCoder 为异步执行，返回值不代表连接是否建立成功，调用 initHardCoder 成功返回 0，若返回ERROR_CODE_NOT_ENABLE = -3，代表 setHcEnable 未设置为 true（默认为 true，上层可用来控制 Hardcoder 是否打开）；

InitHardCoder 连接是否建立成功通过 HardCoderCallback.ConnectStatusCallback 返回值获取：

```
public interface ConnectStatusCallback {
    void onConnectStatus(boolean isConnect);
}
```

根据异步回调 onData 会设置 connect 状态，表示是否已连接上 Server 端；



##### 鉴权 checkPermission

CheckPermission 接口传入厂商名与鉴权值，用于厂商鉴权 APP 是否有权限使用 Hardcoder 功能。目前各厂商鉴权方式不有所区别，具体请见[常见问题](./FAQ.md)1。

CheckPermission 必须在 initHardCoder 建立 socket 成功后调用。此接口用于传输鉴权值给厂商，对需要验证鉴权值的厂商（比如 OPPO），必须调用 checkPermission 把鉴权值传入才可使用 Hardcoder；对没有实现 checkPermission 接口的厂商，系统对 checkPermission 请求没有 callback，因而后续请求调用不应依赖于 checkPermission 的回调，若厂商不返回 checkPermission callback，应也可以调用请求。

```
public static long checkPermission(String[] manufactures, String[] certs, HardCoderCallback.FuncRetCallback callback)
```

manufactures 为字符串数组，传入厂商名；

certs 为字符串数组，传入对应 manufactures 中的厂商的鉴权值；

callback 为 checkPermission 回调：

```
public interface FuncRetCallback {
    void onFuncRet(final int callbackType, final long requestId, final int retCode,
                   final int funcId, final int dataType, final byte[] buffer);
}
```

retCode 代表鉴权是否成功，retCode 为 0 表示成功，否则失败。



##### 请求资源 startPerformance

StartPerformance 用于向系统申请提高 cpu 频率、io 频率、gpu 频率、线程绑核等操作，分别对应 JNI 接口requestCpuHighFreq、requestGpuHighFreq、requestCpuCoreForThread、requestHighIOFreq 以及混合接口 requestUnifyCpuIOThreadCoreGpu，为方便调用，只需要调用统一接口 startPerformance 即可申请所有资源，对不同资源传入不同参数即可。

StartPerformance 在需要申请资源处调用，返回值为请求的 hashcode，上层可保存返回值，在调用stopPerformance 时传入，指定停止此次请求的资源。

```
public static int startPerformance(final int delay, final int cpuLevel, final int ioLevel, final int gpuLevel, final int[] tids, final int timeout, final int scene, final long action, final int callerTid, final String tag);
```

delay 单位 ms，当前请求延迟多久调用，一般为 0。当连续调用 initHardcoder 和 startPerformance 时，若Hardcoder 此时未连接成功，请求会被丢弃，此时可以使用 delay 参数推迟调用；

cpuLevel 本次请求的 cpu 等级，分为：

```
public static final int CPU_LEVEL_0 = 0;//不变
public static final int CPU_LEVEL_1 = 1;//最高频，一般用于重度场景，会把cpu频率提到厂商设置的最高值，适用于能明确知道结束时间的场景（比如启动，进入某个界面，结束后会主动调用stopPerformance）
public static final int CPU_LEVEL_2 = 2;//次高频，一般适用于不知道明确结束时间的场景，比如列表滑动等，不调用stopPerformance，通过手动设置timeout值设置结束时间
public static final int CPU_LEVEL_3 = 3;//也会比正常运行频率要高
```

ioLevel 本次请求的 io 等级，分为：

```
public static final int IO_LEVEL_0 = 0;//不变
public static final int IO_LEVEL_1 = 1;//最高级
public static final int IO_LEVEL_2 = 2;
public static final int IO_LEVEL_3 = 3;
```

gpuLevel 本次请求的 gpu 等级，分为：

```
public static final int GPU_LEVEL_0 = 0;//不变
public static final int GPU_LEVEL_1 = 1;//最高级
```

tids 需要绑定到大核上的线程，为 int[] 数组，可传入单个或多个线程线程号。部分厂商会直接把线程对应的进程的所有线程同时绑定到大核；

timeout 单位 ms，本次请求超时值。若请求未主动调用 stopPerformance，超过 timeout 时间后系统会主动释放资源；若请求超过 timeout 时间仍未被执行（请求过多情况下有可能出现），请求会被移出请求队列；

scene int 值，APP 侧定义的场景值；

action long 值，APP 侧定义的动作值，扩展字段，APP 侧如果只需要使用到 scene 值可以任意传入 action 值；

callerTid 本次请求方的线程号；

tag String 类型，APP 侧定义的 tag，主要用于日志。



##### 结束请求 stopPerformance

场景结束时调用 stopPerformance 释放本次请求资源的调用。

```
public static int stopPerformance(int hashCode);
```

hashCode 对应 startPerformance 的返回值，声明结束本次请求的资源；

返回值表示此次调用是否成功，对应 ERROR_CODE_SUCCESS 和 ERROR_CODE_FAILED。



##### 重要变量

CheckEnv  系统是否支持 Hardcoder，读取配置 persist.sys.hardcoder.name 获取，如果不为空，则手机支持Hardcoder。

HcEnable 上层逻辑是否允许运行 Hardcoder，默认为 true，上层逻辑可以通过 setHcEnable() 设置是否允许使用Hardcoder。

HcDebug 是否允许底层逻辑打印 debug log，默认为 false，可以通过 setHcDebug() 设置。



##### 回调函数

JNI 回调函数：

```
public static void onData(final int callbackType, final long requestId, final long timestamp, final int retCode, final int funcId, final int dataType, final byte[] buffer);
```

callbackType 有两种，CALLBACK_TYPE_STATUS 对应连接状态，此时 buffer 为空；CALLBACK_TYPE_DATA 对应需要参数的回调，此时 buffer 不为空；

```
public final static int CALLBACK_TYPE_BASE = 0;
public final static int CALLBACK_TYPE_STATUS = CALLBACK_TYPE_BASE + 1;
public final static int CALLBACK_TYPE_DATA = CALLBACK_TYPE_BASE + 2;
```

requestId 请求的 request id；

timestamp 回调的时间戳；

retCode 返回值，具体值与 funcId 相关；

funcId 请求的 function id；

dataType 标记 buffer 内容类型；

buffer byte 数组，一般定义为 cJson 值。



##### 注册系统回调函数

```
public static native long registerSystemEventCallback(int tid, long timestamp);
```



##### 注册ANR回调函数

```
public static long registerANRCallback(AnrCallback callback);
```



##### JNI接口

Native 层接口，调用时请确保 checkEnv 为 true。

```
private static native int initHardCoder(String remote, int port, String local);

public static native long checkPermission(String[] manufactures, String[] certs, int pid, int uid, int tid, long timestamp);

public static native long requestCpuHighFreq(int scene, long action, int level, int timeoutms, int tid, long timestamp);

public static native long cancelCpuHighFreq(int tid, long timestamp);

public static native long requestGpuHighFreq(int scene, long action, int level, int timeoutms, int tid, long timestamp);

public static native long cancelGpuHighFreq(int tid, long timestamp);

public static native long configure(byte[] data, int tid, long timestamp);

public static native long requestCpuCoreForThread(int scene, long action, int[] bindtids, int timeoutms, int tid, long timestamp);

public static native long cancelCpuCoreForThread(int[] bindtids, int tid, long  timestamp);

public static native long requestHighIOFreq(int scene, long action, int level, int timeoutms, int tid, long timestamp);

public static native long cancelHighIOFreq(int tid, long timestamp);

public static native long requestScreenResolution(int level, String uiName, int tid, long timestamp);

public static native long resetScreenResolution(int tid, long timestamp);

public static native long registerANRCallback(int tid, long timestamp);

public static native long registerSystemEventCallback(int tid, long timestamp);

public static native long registerBootPreloadResource(String[] files, int tid, long timestamp);

public static native long terminateApp(int tid, long timestamp);

public static native long requestUnifyCpuIOThreadCoreGpu(int scene, long action, int cpulevel, int gpulevel, int iolevel, int[] bindtids, int timeoutms, int tid, long  timestamp);

public static native long cancelUnifyCpuIOThreadCoreGpu(boolean cancelcpu, boolean cancelgpu, boolean cancelio, boolean cancelthread, int[] bindtids, int tid, long timestamp);

private static native void setHCEnable(boolean enable);

public static native void setRetryConnectInterval(int interval);

private static native void setDebug(boolean debug);

public static native int startTraceCpuLoad(int linuxTid, int sampleRate);

public static native int stopTraceCpuLoad(int id);

public static native int isRunning();

private static native int getTickRate();
```



### 统计

`HardCoderReporter.java` 中提供了对 startPerformance 接口请求的基本数据统计，APP 可以通过HardCoderReporter.setReporter 传入统计接口 HardCoderReporterInterface 的实现类获取请求调用的具体运行状态。

HardCoderReporterInterface 包括两个函数，其中 PerformanceReport 为正常请求的调用情况回调：

```
void performanceReport(int[] performanceBindCoreThreadId, int enable, int engineStatus, int cancelInDelay, int scene, long action, int cpuLevel, int ioLevel, int[] bindCoreThreadIdArray, int executeTime, int runtime, int phoneHZ, int[] cpuLevelTimeArray, int[] ioLevelTimeArray);
```

enable 应用是否打开 Hardcoder 开关，用 HardCoderJNI.setHcEnable 设置；

engineStatus 手机是否支持 Hardcoder；

cancelInDelay 请求是否在 delay 时间内未被执行就取消；

scene 场景值；

action 操作值，保留字段；

cpuLevel 请求的 CPU level；

ioLevel 请求的 IO level；

bindCoreThreadIdArray 本次请求绑核的线程 id，可能有多个，为字符串，以"#"分割；

executeTime 请求执行时间，stopTime - startTime，不包括 delay 时间；

runtime 请求运行时间，stopTime-initTime，包括 delay 时间；

phoneHZ 手机时钟周期中断次数，单位时间内 Tick 节拍数，对应 HardCoderJNI.tickRate；

cpuLevelTimeArray 各个 cpu level 的持续时间，为字符串，以"#"分割；

ioLevelTimeArray 各个 io level 的持续时间时间，为字符串，以"#"分割。



ErrorReport 为错误情况下的回调：

```
void errorReport(int callbackType, long errorTimestamp, int errCode, int funcId, int dataType);
```

callbackType 错误类型；

errorTimestamp 发生错误时间；

errCode 错误代码；

funcId 请求的 function id；

dataType 返回的数据类型。