# Hardcoder

[![license](http://img.shields.io/badge/license-BSD3-brightgreen.svg?style=flat)](https://github.com/Tencent/tinker/blob/master/LICENSE)[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/Tencent/tinker/pulls)



中文版请见[这里](https://github.com/Tencent/Hardcoder/wiki/Home)。



**Hardcoder is a solution which allows Android APP and Android System to communicate with each other directly, solving the problem that Android APP could only use system standard API rather than the hardware resources of the system. Through Hardcoder, Android APP can make good use of mobile phones hardware resources such as CPU frequency, Large Core, and the GPU to improve APP performance. Hardcoder allows Android system to get more information from APP in order to better provide system resources to Android APP. At the same time, for lack of implementation by the standard interface, the APP and the system can also realize the model adaptation and function expansion through the framework.**



Hardcoder framework can averagely optimize the performance of Wechat by 10%-30% in terms of Wechat startup, video delivery, mini program startup, and other highly-loaded scenes. Furthermore, it could also averagely optimize the performance of Mobile QQ by 10%-50% in terms of mobile QQ startup, chatting Initialization, picture delivery, and other highly-loaded scenes. The framework now has been applied to mobile brands such as OPPO, vivo, Huawei, XIAOMI, Samsung, Meizu, etc and covers more than 460 millions devices.

![readme](https://github.com/Tencent/Hardcoder/wiki/images/readme.jpg)



## Getting started

1.  Read “[Product introduction of Hardcoder](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-产品方案介绍)” to learn about Hardcoder.

2. Read “[Technical introduction of Hardcoder](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-技术方案介绍)” to know the implementation philosophy and technical framework.

3. Use the testapp to quickly verify the performance of Hardcoder. For the further detail, please check ”[Hardcoder testapp testing instruction](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-Testapp测试指南)“ and “[Hardcoder Benchmark](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-Benchmark)”.

4. Please check the “[Hardcoder Application Instruction](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-接入指南)” to learn how to use Hardcoder.

   1. Download Hardcoder repo and compline Hardcoder aar.
   2. Apply Hardcoder aar to “build.gradle”.
   3. Call initHardCoder to establish socket connection when process initializes (Generally, it needs to request resource when process initializes. That is the reason why to call initHardCoder when process initializes). Every process is individual and they all need to call initHardCoder to establish socket connection. Every process keeps a socket after the connection and the socket will disconnect if the process quits.
   4. Call checkPermission after the success of InitHardCoder call-back and transfer authentication values which are applied from different mobile brands by APP.
   5. Call startPerformance under the condition of resource request scenes and transfer parameters that request resource. If the scene is in the stage of process initiation, for example APP startup, startPerformance should not be called until it successfully calls back initHardCoder or it needs to verify whether socket is connected by examining isConnect() of HardCoderJNI.
   6. Actively call stopPerformance when scene stops and it needs to transfer the “hashCode" corresponding to the startPerformance in order to identify the corresponding scene. Then it can stop this request.
   7. Test the performance. To do the comparison between the situation in which “Hardcoder is on and  off”.

5. Apply the authentication from mobile brands. For the further detail, please check [FAQ](https://github.com/Tencent/Hardcoder/wiki/FAQ).

6. Launch APP which has involved Hardcoder.

   

## Document Support

1. Product introduction of Hardcoder——[https://github.com/Tencent/Hardcoder/wiki/Hardcoder-产品方案介绍](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-产品方案介绍)
2. Technical introduction of Hardcoder——[https://github.com/Tencent/Hardcoder/wiki/Hardcoder-技术方案介绍](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-技术方案介绍)
3. Hardcoder testapp testing instruction——[https://github.com/Tencent/Hardcoder/wiki/Hardcoder-Testapp测试指南](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-Testapp测试指南)
4. Hardcoder Application Instruction——[https://github.com/Tencent/Hardcoder/wiki/Hardcoder-接入指南](https://github.com/Tencent/Hardcoder/wiki/Hardcoder-接入指南)
5. FAQ——https://github.com/Tencent/Hardcoder/wiki/FAQ
6. Hardcoder for Android API References——https://tencent.github.io/Hardcoder/
7. Hardcoder Benchmark——https://github.com/Tencent/Hardcoder/wiki/Hardcoder-Benchmark




## License

Hardcoder is under the BSD license. See the [LICENSE](https://github.com/Tencent/Hardcoder/blob/master/LICENSE) file for details.


## Personal Information Protection Rules

https://support.weixin.qq.com/cgi-bin/mmsupportacctnodeweb-bin/pages/kGLpLlCX1Vkskw7U


If you have any questions，welcome to join QQ group to contact us.

![qqgroup_qrcode.png](https://github.com/Tencent/Hardcoder/wiki/images/qqgroup_qrcode.png)



## 信息公示

- SDK 名称：Hardcoder
- 版本号：v1.0.0
- 开发者：深圳市腾讯计算机系统有限公司
- 主要功能：Hardcoder 是一套 Android APP 与系统间的通信解决方案，突破了 APP 只能调用系统标准 API，无法直接调用系统底层硬件资源的问题，让 Android APP 和系统能实时通信。
- [Hardcoder SDK 使用说明](https://github.com/Tencent/Hardcoder/wiki)
- [Hardcoder SDK 个人信息保护规则](https://support.weixin.qq.com/cgi-bin/mmsupportacctnodeweb-bin/pages/kGLpLlCX1Vkskw7U)
