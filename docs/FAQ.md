# Hardcoder 常见问题



1. ### 开发者接入 Hardcoder 如何获取厂商权限？

   为方便开发者接入，我们努力与各厂商进行沟通，建立一套尽可能自助申请权限的机制。

   | 厂商 | 无需注册 | 自助注册 | 需要申请 | 申请方式                                                     |
   | :--: | :------: | :------: | :------: | ------------------------------------------------------------ |
   | OPPO |          |    √     |          | OPPO 开放平台官网 https://open.oppomobile.com/wiki/doc#id=10355 注册获取 APP 授权码，通过 checkPermission 接口传入 |
   | vivo |          |          |    √     | 未来会搭建开放平台，目前需要通过商务渠道 open@vivo.com 获得vivo认证。 |
   | 小米 |          |          |    √     | APP 接入后根据 APP 测试数据判定是否开放权限，邮件联系：guyunjian@xiaomi.com，liucai@xiaomi.com |
   | 华为 |    √     |          |          |                                                              |
   | 三星 |    √     |          |          |                                                              |

   厂商列表会随着业务的发展持续更新。

   

2. ### Hardcoder 会造成系统资源抢占或者浪费吗？

   Hardcoder 所有调用只是应用发出的请求，代表应用此时需要资源，但是具体响应取决于系统。比如在系统资源紧张时，系统有可能无法分配相应资源。

   同时，Hardcoder server 端也会对应用请求资源做一定限制（厂商侧），确保不会造成资源抢占问题，比如只有应用在前台时允许请求资源，在后台时请求会被忽略；限制应用请求资源的最长时间等。

   

4. ### Hardcoder 覆盖了哪些机型，开发者需要做适配吗？

   目前 OPPO、vivo、华为、小米、三星、魅族等主流手机厂商已支持 Hardcoder，覆盖 4.6+ 亿设备量。调用 initHardcoder 接口创建 socket 连接时可判断该手机是否支持 Hardcoder，若不支持，会设置标志位，后续所有请求调用会被拦截，确保不影响在不支持 Hardcoder 机型上的性能。开发者无需再做适配。

   

5. ### Hardcoder 的请求会阻塞吗？

   Hardcoder 本质上只是应用提出资源诉求，所有调用为异步调用，不阻塞。一般情况下应用无需关心请求是否被响应，若需要得到请求是否被执行，可通过异步回调获取。

   

5. ### 场景值需要与厂商协商吗？

   场景值由 APP 自行定义，推荐使用 Hardcoder 分类定义，代码在 `HardCoderJNI.java`。

       public final static int APP_SCENE_UNDEFINE = 0;                 //未定义场景
       public final static int APP_SCENE_STARTUP = 1;                  //进程启动，APP启动
       public final static int APP_SCENE_WINDOW_SWITCH = 2;            //UI页面切换（同一进程），activity，fragment切换
       public final static int APP_SCENE_WINDOW_SCROLL = 3;            //UI页面滑动
       public final static int APP_SCENE_DATA_LOADING_AND_PROCESS = 4; //数据加载和处理任务，指APP本地前后台任务
       public final static int APP_SCENE_MULTI_MEDIA_PROCESS = 5;      //多媒体相关业务
       public final static int APP_SCENE_COMMUNICATE = 6;              //APP/服务端通信
       public final static int APP_SCENE_SYSTEM_DEVICE = 7;            //调用系统服务
   