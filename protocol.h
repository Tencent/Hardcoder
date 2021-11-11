/**
 * protocal const definition and implement
 */
#ifndef AMC_PROTOCOL_H
#define AMC_PROTOCOL_H

#include "amc.pb.h"
#include "header.h"
#include <cJSON.h>

// LEVEL
static const uint32_t CPU_LEVEL_0 = 0; // not change
static const uint32_t CPU_LEVEL_1 = 1; // highest
static const uint32_t CPU_LEVEL_2 = 2;
static const uint32_t CPU_LEVEL_3 = 3;

static const uint32_t GPU_LEVEL_0 = 0; // not change
static const uint32_t GPU_LEVEL_1 = 1; // highest

static const uint32_t IO_LEVEL_0 = 0; // not change
static const uint32_t IO_LEVEL_1 = 1; // highest
static const uint32_t IO_LEVEL_2 = 2;
static const uint32_t IO_LEVEL_3 = 3;

static const uint32_t JSON_VERSION = 1;

// HC register call back type
static const uint32_t REGISTER_CALLBACK_TYPE_BASE = 0x0;
static const uint32_t REGISTER_CALLBACK_TYPE_SYSTEM_EVENT = REGISTER_CALLBACK_TYPE_BASE + 1;
static const uint32_t REGISTER_CALLBACK_TYPE_ANR = REGISTER_CALLBACK_TYPE_BASE + 2;
static const uint32_t REGISTER_CALLBACK_TYPE_CPU_LOAD = REGISTER_CALLBACK_TYPE_BASE + 3;
static const uint32_t REGISTER_CALLBACK_TYPE_DEVICE_TEMPERATURE= REGISTER_CALLBACK_TYPE_BASE + 4;


// HC system event definition
static const uint32_t SYSTEM_EVENT_BASE = 0x0;
static const uint32_t SYSTEM_EVENT_SLIDE_OPEN = SYSTEM_EVENT_BASE + 1;
static const uint32_t SYSTEM_EVENT_SLIDE_CLOSE = SYSTEM_EVENT_BASE + 2;
static const uint32_t SYSTEM_EVENT_BLUETOOTH_ACCEPTCALL = SYSTEM_EVENT_BASE + 3;
static const uint32_t SYSTEM_EVENT_BLUETOOTH_HANGUPCALL = SYSTEM_EVENT_BASE + 4;


static const int32_t RET_OK = 0;

//requestCpuHighFreq，requestHighIOFreq 直接返回level n
static const int32_t RET_LEVEL_1 = 1;
static const int32_t RET_LEVEL_2 = 2;
static const int32_t RET_LEVEL_3 = 3;

//预留返回值最后三位作为level，倒数第四位代表cpu level，倒数第五位代表io level，新增值继续左移
static const int32_t RET_CPU_HIGH_FREQ = 1 << 3;// 1000，即8
static const int32_t RET_HIGH_IO_FREQ = 1 << 4; // 10000，即16
static const int32_t RET_GPU_HIGH_FREQ = 1 << 5; // 100000，即32, GPU只有level 1和0，故GPU可以不使用level位，直接用100000表示level 1


//requestUnifyCpuIOThreadCoreGpu使用复合标识位
static const int32_t RET_CPU_HIGH_FREQ_LEVEL_1 = RET_CPU_HIGH_FREQ | RET_LEVEL_1;   //Unify接口返回cpu level 1，1000 | 01 = 1001
static const int32_t RET_CPU_HIGH_FREQ_LEVEL_2 = RET_CPU_HIGH_FREQ | RET_LEVEL_2;   //Unify接口返回cpu level 2，1000 | 10 = 1010
static const int32_t RET_CPU_HIGH_FREQ_LEVEL_3 = RET_CPU_HIGH_FREQ | RET_LEVEL_3;   //Unify接口返回cpu level 3，1000 | 11 = 1011

static const int32_t RET_HIGH_IO_FREQ_LEVEL_1 = RET_HIGH_IO_FREQ | RET_LEVEL_1;     //Unify接口返回io level 1，10000 | 01 = 10001
static const int32_t RET_HIGH_IO_FREQ_LEVEL_2 = RET_HIGH_IO_FREQ | RET_LEVEL_2;     //Unify接口返回io level 2，10000 | 10 = 10010
static const int32_t RET_HIGH_IO_FREQ_LEVEL_3 = RET_HIGH_IO_FREQ | RET_LEVEL_3;     //Unify接口返回io level 3，10000 | 11 = 10011


static const int32_t ERR_UNAUTHORIZED = -10001;
static const int32_t ERR_FUNCTION_NOT_SUPPORT = -10002;
static const int32_t ERR_SERVICE_UNAVAILABLE = -10003;
static const int32_t ERR_FAILED_DEPENDENCY = -10004;
static const int32_t ERR_PACKAGE_DECODE_FAILED = -10005;
static const int32_t ERR_PARAMETERS_WRONG = -10006;
static const int32_t ERR_CLIENT_UPGRADE_REQUIRED = -10007;

static const int32_t ERR_CLIENT_CONNECT = -20000;
static const int32_t ERR_CLIENT_DISCONNECT = -20001;
static const int32_t ERR_CLIENT_ALREADY_INIT = -20002;


// FUNC ID BEGIN  ,  2 ^ 16 IS MAX
static const uint32_t FUNC_BASE = 1000;

static const uint32_t FUNC_CHECK_PERMISSION = FUNC_BASE + 1;

static const uint32_t FUNC_CPU_HIGH_FREQ = FUNC_BASE + 2;
static const uint32_t FUNC_CANCEL_CPU_HIGH_FREQ = FUNC_BASE  + 3;

static const uint32_t FUNC_CPU_CORE_FOR_THREAD = FUNC_BASE + 4;
static const uint32_t FUNC_CANCEL_CPU_CORE_FOR_THREAD = FUNC_BASE + 5;

static const uint32_t FUNC_HIGH_IO_FREQ = FUNC_BASE + 6;
static const uint32_t FUNC_CANCEL_HIGH_IO_FREQ = FUNC_BASE + 7;

static const uint32_t FUNC_SET_SCREEN_RESOLUTION = FUNC_BASE + 8;
static const uint32_t FUNC_RESET_SCREEN_RESOLUTION = FUNC_BASE + 9;

static const uint32_t FUNC_REG_ANR_CALLBACK = FUNC_BASE + 10;
static const uint32_t FUNC_REG_PRELOAD_BOOT_RESOURCE = FUNC_BASE + 11;

static const uint32_t FUNC_TERMINATE_APP = FUNC_BASE + 12;

static const uint32_t FUNC_UNIFY_CPU_IO_THREAD_CORE_GPU = FUNC_BASE + 13;
static const uint32_t FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE_GPU = FUNC_BASE + 14;

static const uint32_t FUNC_REG_SYSTEM_EVENT_CALLBACK = FUNC_BASE + 15;

static const uint32_t FUNC_GPU_HIGH_FREQ = FUNC_BASE + 16;
static const uint32_t FUNC_CANCEL_GPU_HIGH_FREQ = FUNC_BASE + 17;

static const uint32_t FUNC_CONFIGURE = FUNC_BASE + 18;

static const uint32_t FUNC_GET_PARAMETERS = FUNC_BASE + 19;

static const uint32_t FUNC_REG_CPU_LOAD_CALLBACK = FUNC_BASE + 20;
static const uint32_t FUNC_REG_DEVICE_TEMPERATURE_CALLBACK = FUNC_BASE + 21;
static const uint32_t FUNC_GET_CURRENT_TEMPERATURES_WITH_TYPE = FUNC_BASE + 22;


static const uint32_t STATUS_NONE = 0;
static const uint32_t STATUS_REQUEST = 1;
static const uint32_t STATUS_RESET = 2;

// FUNC ID END  ,  2 ^ 16 IS MAX

static const uint32_t CALLBACK_TYPE_BASE = 0;
static const uint32_t CALLBACK_TYPE_STATUS = CALLBACK_TYPE_BASE + 1;
static const uint32_t CALLBACK_TYPE_DATA = CALLBACK_TYPE_BASE + 2;


//HardCoder interface
class HardCoder {
public:
    virtual ~HardCoder(){}

    virtual int getUidByAddress(const char *ip, int port) {
        UNUSED(ip);
        UNUSED(port);
        return 0;
    }

    virtual bool checkPermission(std::vector<std::string> manufactures, std::vector<std::string> certs, int funcid, int uid, int callertid, int64_t timestamp) {
        UNUSED(manufactures);
        UNUSED(certs);
        UNUSED(funcid);
        UNUSED(uid);
        UNUSED(callertid);
        UNUSED(timestamp);
        return true;
    }

    virtual int requestCpuHighFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        UNUSED(scene);
        UNUSED(action);
        UNUSED(level);
        UNUSED(timeoutms);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int cancelCpuHighFreq(int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int requestGpuHighFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        UNUSED(scene);
        UNUSED(timeoutms);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int cancelGpuHighFreq(int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int requestCpuCoreForThread(int scene, int64_t action, std::vector<int>bindtids, int timeoutms, int callertid, int64_t timestamp) {
        UNUSED(scene);
        UNUSED(action);
        UNUSED(bindtids);
        UNUSED(timeoutms);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int cancelCpuCoreForThread(std::vector<int>bindtids, int callertid, int64_t timestamp) {
        UNUSED(bindtids);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int requestHighIOFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        UNUSED(scene);
        UNUSED(action);
        UNUSED(level);
        UNUSED(timeoutms);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int cancelHighIOFreq(int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int requestScreenResolution(int level, std::string uiName, int callertid, int64_t timestamp) {
        UNUSED(level);
        UNUSED(uiName);
        UNUSED(level);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int resetScreenResolution(int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int registerAnrCallback(int uid, int callertid, int64_t timestamp) {
        UNUSED(uid);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int registerSystemEventCallback(int uid, int callertid, int64_t timestamp) {
        UNUSED(uid);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int registerBootPreloadResource(std::vector<std::string> file, int callertid, int64_t timestamp) {
        UNUSED(file);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int terminateApp(int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int requestUnifyCpuIOThreadCoreGpu(int scene, int64_t action, int cpulevel, int iolevel, std::vector<int>bindtids, int gpulevel, int timeoutms, int callertid, int64_t timestamp) {
        UNUSED(scene);
        UNUSED(action);
        UNUSED(cpulevel);
        UNUSED(iolevel);
        UNUSED(bindtids);
        UNUSED(gpulevel);
        UNUSED(timeoutms);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int cancelUnifyCpuIOThreadCoreGpu(int cancelcpu, int cancelio, int cancelthread, std::vector<int>bindtids, int cancelgpu, int callertid, int64_t timestamp) {
        UNUSED(cancelcpu);
        UNUSED(cancelio);
        UNUSED(cancelthread);
        UNUSED(bindtids);
        UNUSED(cancelgpu);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int configure(const std::string& data, int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int getParameters(const std::string& data, int callertid, int64_t timestamp) {
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int registerCpuLoadCallback(int uid, int callertid, int64_t timestamp) {
        UNUSED(uid);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int registerDeviceTemperatureCallback(int uid, int callertid, int64_t timestamp) {
        UNUSED(uid);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    virtual int getCurrentTemperaturesWithType(int type, int callertid, int64_t timestamp) {
        UNUSED(type);
        UNUSED(callertid);
        UNUSED(timestamp);
        return ERR_FUNCTION_NOT_SUPPORT;
    }

};

class IHardCoderDataCallback{
public:
    virtual int onCallback(int uid, int funcid, uint64_t requestid, int bodyFormat, uint64_t timestamp, int retCode, uint8_t *data, int len) = 0;
    virtual ~IHardCoderDataCallback(){}
};

template<typename T>
class ServerDataWorker {

private:
    T remote;
    uint8_t *resp;
    uint32_t respLen;

public:
    ServerDataWorker():remote() {
        resp = NULL;
        respLen = 0;
    }

    ~ServerDataWorker() {
        if (resp) {
            delete[] resp;
        }
        resp = NULL;
    }

    uint8_t* getResp() {
        return resp;
    }

    uint32_t getResLen() {
        return respLen;
    }

    T getRemote() {
        return remote;
    }

    int64_t processSend(uint32_t funcid, T *r, uint8_t *data, uint32_t len) {//server send
        remote = *r;
        int64_t requestId = genRespPack(funcid, RET_OK, 0, data, len,  &resp, &respLen);
        pdbg("processSend funcid:%d, len:%d, remote:%d requestId:%lld, respLen:%d", funcid, len, remote, requestId, respLen);
        return requestId;
    }

    // #lizard forgives
    int64_t processReceive(int uid, T *r,  AMCReqHeader *pHead,  uint8_t *payload, uint32_t payloadLen, HardCoder *inst) {
        if (r == NULL || (payload == NULL && payloadLen != 0)) {
            perr("recvEvent DATA data or addr invalid ret len:%d, %d", payloadLen, TOINT(sizeof(AMCReqHeader)));
            return -1;
        }

        remote = *r;

        if (pHead->version < HEADER_PROTOCAL_VERSION) {
            perr("CheckVersion failed:%d  server:%d", pHead->version, HEADER_PROTOCAL_VERSION);
            return genRespPack(pHead->funcid, ERR_CLIENT_UPGRADE_REQUIRED, pHead->requestid, NULL, 0, &resp, &respLen);;
        }

//        if (!inst->checkPermission(pHead->funcid, uid, pHead->callertid, pHead->timestamp)) {
//            perr("checkPermission[%d, %d] failed ret ERR_UNAUTHORIZED", pHead->funcid, uid);
//            genRespPack(pHead->funcid, ERR_UNAUTHORIZED, 0, NULL, 0, &resp, &respLen);
//            return 0;
//        }

        if (pHead->funcid == FUNC_CHECK_PERMISSION) {
            amc::CheckPermission p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<std::string> vec1;
            std::vector<std::string> vec2;
            if (ret) {
                int size = p.certtags_size();
                for (int i = 0; i < size; i++) {
                    vec1.push_back(p.certtags(i).manufacture());
                    vec2.push_back(p.certtags(i).cert());
                }
            }
            int respResult = inst->checkPermission(vec1, vec2, pHead->funcid, uid, pHead->callertid, pHead->timestamp) ? 0 : ERR_UNAUTHORIZED;
            pdbg("FUNC_CHECK_PERMISSION [%d,%d] resp:%d", pHead->funcid, uid, respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);;
        }

        if (pHead->funcid == FUNC_UNIFY_CPU_IO_THREAD_CORE_GPU) {
            amc::RequestUnifyCPUIOThreadCore p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<int> vec;
            if (ret) {
                int size = p.bindtids_size();
                for (int i = 0; i < size; i++) {
                    vec.push_back(p.bindtids(i));
                }
            }
            int respResult = ret ? inst->requestUnifyCpuIOThreadCoreGpu(p.scene(),p.action(), p.cpulevel(), p.iolevel(), vec, p.gpulevel(), p.timeoutms(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_UNIFY_CPU_IO_THREAD_CORE parse:%d [%d,%d,%d,%d,%d,%d,%d] resp:%d", ret, p.scene(), TOINT(p.action()), p.cpulevel(), p.iolevel(), TOINT(p.bindtids_size()), p.gpulevel(), p.timeoutms(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE_GPU) {
            amc::CancelUnifyCPUIOThreadCore p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<int> vec;
            if (ret) {
                int size = p.bindtids_size();
                for (int i = 0; i < size; i++) {
                    vec.push_back(p.bindtids(i));
                }
            }
            int respResult = ret ? inst->cancelUnifyCpuIOThreadCoreGpu(p.cancelcpu(), p.cancelio(), p.cancelthread(), vec,p.cancelgpu(),  pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE parse:%d [%d,%d,%d,%d,%d] resp:%d", ret, p.cancelcpu(), p.cancelio(), p.cancelthread(), TOINT(p.bindtids_size()), p.cancelgpu(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CPU_HIGH_FREQ) {
            amc::RequestCPUHighFreq p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = ret ? inst->requestCpuHighFreq(p.scene(),p.action(), p.level(), p.timeoutms(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_CPU_HIGH_FREQ parse:%d [%d,%d,%d,%d] resp:%d", ret, p.scene(), TOINT(p.action()), p.level(), TOINT(p.timeoutms()), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CANCEL_CPU_HIGH_FREQ) {
            int respResult = inst->cancelCpuHighFreq(pHead->callertid, pHead->timestamp);
            pdbg("FUNC_CANCEL_CPU_HIGH_FREQ resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_GPU_HIGH_FREQ) {
            amc::RequestGPUHighFreq p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = ret ? inst->requestGpuHighFreq(p.scene(),p.action(), p.level(), p.timeoutms(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_GPU_HIGH_FREQ parse:%d [%d,%d,%d,%d] resp:%d", ret, p.scene(), TOINT(p.action()), p.level(), TOINT(p.timeoutms()), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CANCEL_GPU_HIGH_FREQ) {
            int respResult = inst->cancelGpuHighFreq(pHead->callertid, pHead->timestamp);
            pdbg("FUNC_CANCEL_GPU_HIGH_FREQ resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CPU_CORE_FOR_THREAD) {
            amc::RequestCPUCoreForThread p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<int> vec;
            if (ret) {
                int size = p.bindtids_size();
                for (int i = 0; i < size; i++) {
                    vec.push_back(p.bindtids(i));
                }
            }
            int respResult = ret ? inst->requestCpuCoreForThread(p.scene(), p.action(), vec, p.timeoutms(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_CPU_CORE_FOR_THREAD  parse:%d [%d,%d,%d,%d] resp:%d", ret, p.scene(),TOINT(p.action()), p.bindtids_size(), TOINT(p.timeoutms()), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CANCEL_CPU_CORE_FOR_THREAD) {
            amc::CancelCPUCoreForThread p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<int> vec;
            if (ret) {
                int size = p.bindtids_size();
                for (int i = 0; i < size; i++) {
                    vec.push_back(p.bindtids(i));
                }
            }
            int respResult = ret ? inst->cancelCpuCoreForThread(vec, pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_CANCEL_CPU_CORE_FOR_THREAD  parse:%d [%d] resp:%d", ret, p.bindtids_size(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_HIGH_IO_FREQ) {
            amc::RequestHighIOFreq p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = ret ? inst->requestHighIOFreq(p.scene(), p.action(), p.level(), p.timeoutms(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_HIGH_IO_FREQ  parse:%d [%d,%d,%d,%d] resp:%d", ret, p.scene(), TOINT(p.action()), p.level(), TOINT(p.timeoutms()), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CANCEL_HIGH_IO_FREQ) {
            int respResult = inst->cancelHighIOFreq(pHead->callertid, pHead->timestamp);
            pdbg("FUNC_CANCEL_HIGH_IO_FREQ resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_SET_SCREEN_RESOLUTION) {
            amc::RequestScreenResolution p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = ret ? inst->requestScreenResolution(p.level(), p.uiname(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_SET_SCREEN_RESOLUTION  parse:%d [%d,%s] resp:%d", ret, p.level(), p.uiname().c_str(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_RESET_SCREEN_RESOLUTION) {
            int respResult = inst->resetScreenResolution(pHead->callertid, pHead->timestamp);
            pdbg("FUNC_RESET_SCREEN_RESOLUTION resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_REG_SYSTEM_EVENT_CALLBACK) {
            amc::RegisterCallback p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult =  inst->registerSystemEventCallback(p.uid(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_REG_SYSTEM_EVENT_CALLBACK  parse:%d [%d,%d,%d] resp:%d", ret, p.type(), p.uid(), p.pid(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_REG_ANR_CALLBACK) {
            amc::RegisterCallback p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult =  inst->registerAnrCallback(p.uid(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_REG_ANR_CALLBACK  parse:%d [%d,%d,%d] resp:%d", ret, p.type(), p.uid(), p.pid(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_REG_PRELOAD_BOOT_RESOURCE) {
            amc::RequestBootPreLoadResource p;
            int ret = p.ParseFromArray(payload, payloadLen);
            std::vector<std::string> vec;
            if (ret) {
                int size = p.filelist_size();
                for (int i = 0; i < size; i++) {
                    vec.push_back(p.filelist(i));
                }
            }
            int respResult = ret ? inst->registerBootPreloadResource(vec, pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_REG_PRELOAD_BOOT_RESOURCE  parse:%d vecSize:%d resp:%d", ret, TOINT(vec.size()), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_TERMINATE_APP) {
            int respResult = inst->terminateApp(pHead->callertid, pHead->timestamp);
            pdbg("FUNC_TERMINATE_APP resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_CONFIGURE) {
            amc::Configuration p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = inst->configure(p.data(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_CONFIGURE resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_GET_PARAMETERS) {
            amc::GetParameters p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = inst->getParameters(p.data(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_GET_PARAMETERS resp:%d", respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_REG_CPU_LOAD_CALLBACK) {
            amc::RegisterCallback p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult =  inst->registerCpuLoadCallback(p.uid(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_REG_CPU_LOAD_CALLBACK  parse:%d [%d,%d,%d] resp:%d", ret, p.type(), p.uid(), p.pid(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_REG_DEVICE_TEMPERATURE_CALLBACK) {
            amc::RegisterCallback p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult =  inst->registerDeviceTemperatureCallback(p.uid(), pHead->callertid, pHead->timestamp);
            pdbg("FUNC_REG_DEVICE_TEMPERATURE_CALLBACK  parse:%d [%d,%d,%d] resp:%d", ret, p.type(), p.uid(), p.pid(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }

        if (pHead->funcid == FUNC_GET_CURRENT_TEMPERATURES_WITH_TYPE) {
            amc::GetCurrentTemperaturesWithType p;
            int ret = p.ParseFromArray(payload, payloadLen);
            int respResult = ret ? inst->getCurrentTemperaturesWithType(p.type(), pHead->callertid, pHead->timestamp) : ERR_PACKAGE_DECODE_FAILED;
            pdbg("FUNC_GET_CURRENT_TEMPERATURES_WITH_TYPE parse:%d [%d] resp:%d", ret, p.type(), respResult);
            return genRespPack(pHead->funcid, respResult, pHead->requestid, NULL, 0, &resp, &respLen);
        }
        return genRespPack(pHead->funcid, ERR_FUNCTION_NOT_SUPPORT, pHead->requestid, NULL, 0, &resp, &respLen);
    }
};


class IClientEngine {
public:
    virtual bool isRunning() = 0;
    virtual int64_t sendReq(int funcid, uint8_t *data, int dataLen, int tid, int64_t timestamp) = 0;
    virtual  ~IClientEngine(){}
};

class IC2JavaCallback {
public:
    virtual int callback(int callbackType, uint64_t requestid, uint64_t timestamp, int errCode, int funcid, int dataType,
             uint8_t *data, int len) = 0;
    virtual ~IC2JavaCallback(){}
};

class ClientProtocal : public IHardCoderDataCallback {

protected:
    virtual IClientEngine* engine() = 0;

    virtual IC2JavaCallback* c2JavaCallback() = 0;

public:
    ClientProtocal() {}

    int64_t checkPermission(std::vector<std::string> manufactures, std::vector<std::string> certs, int pid, int uid, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CHECK_PERMISSION);

            if (!manufactures.empty() && manufactures.size() == certs.size()) {
                cJSON *certTags;
                cJSON_AddItemToObject(jsonRequest, "certTags", certTags = cJSON_CreateArray());
                for (int index = 0; index < manufactures.size(); index++) {
                    cJSON *certTag;
                    certTag = cJSON_CreateObject();
                    cJSON_AddStringToObject(certTag, "manufacture", manufactures[index].c_str());
                    cJSON_AddStringToObject(certTag, "cert", certs[index].c_str());
                    cJSON_AddItemToArray(certTags, certTag);
                }
            }

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CHECK_PERMISSION, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::CheckPermission request;
            if (!manufactures.empty() && manufactures.size() == certs.size()) {
                for (int index = 0; index < manufactures.size(); index++) {
                    amc::CertTag* certTag = request.add_certtags();
                    certTag->set_manufacture(manufactures[index]);
                    certTag->set_cert(certs[index]);
                }
            }
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_CHECK_PERMISSION, body, len, tid, timestamp);
        }
        pdbg("checkPermission requestid:%lld, pid:%d, uid:%d, local pid:%d, uid:%d, tid:%d, timestamp:%d", requestid,
             pid, uid, getpid(), getuid(), tid, TOINT(timestamp / 1000000L));
        return requestid;
    }

    int64_t requestCpuHighFreq(int scene, int64_t action, int level, int timeoutms, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CPU_HIGH_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "scene", scene);
            cJSON_AddNumberToObject(jsonRequest, "action", action);
            cJSON_AddNumberToObject(jsonRequest, "cpulevel", level);
            cJSON_AddNumberToObject(jsonRequest, "timeoutMs", timeoutms);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CPU_HIGH_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::RequestCPUHighFreq request;
            request.set_scene(scene);
            request.set_level(level);
            request.set_timeoutms(timeoutms);
            request.set_action(action);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_CPU_HIGH_FREQ, body, len, tid, timestamp);
        }

        pdbg("requestCpuHighFreq requestid:%lld, scene:%d, action:%d, level:%d, timeoutms:%d, tid:%d, timestamp:%d" ,
             requestid, scene, TOINT(action), level, timeoutms, tid, TOINT(timestamp/1000000L));
        return requestid;
    }

    int64_t cancelCpuHighFreq(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CANCEL_CPU_HIGH_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);
            cJSON_AddNumberToObject(jsonRequest, "cpulevel", CPU_LEVEL_0);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CANCEL_CPU_HIGH_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            requestid = pEngine->sendReq(FUNC_CANCEL_CPU_HIGH_FREQ, NULL, 0, tid, timestamp);
        }
        pdbg("cancelCpuHighFreq requestid:%lld, tid:%d, timestamp:%d", requestid, tid, TOINT(timestamp / 1000000L));
        return requestid;
    }

    int64_t requestGpuHighFreq(int scene, int64_t action, int level, int timeoutms, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_GPU_HIGH_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "scene", scene);
            cJSON_AddNumberToObject(jsonRequest, "action", action);
            cJSON_AddNumberToObject(jsonRequest, "gpulevel", level);
            cJSON_AddNumberToObject(jsonRequest, "timeoutMs", timeoutms);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_GPU_HIGH_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RequestGPUHighFreq request;
            request.set_scene(scene);
            request.set_level(level);
            request.set_timeoutms(timeoutms);
            request.set_action(action);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_GPU_HIGH_FREQ, body, len, tid, timestamp);
        }
        pdbg("requestGpuHighFreq requestid:%lld, scene:%d, action:%d, level:%d, timeoutms:%d, tid:%d, timestamp:%d" ,
             requestid, scene, TOINT(action), level, timeoutms, tid, TOINT(timestamp/1000000L));
        return requestid;
    }


    int64_t cancelGpuHighFreq(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CANCEL_GPU_HIGH_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);
            cJSON_AddNumberToObject(jsonRequest, "gpulevel", GPU_LEVEL_0);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CANCEL_GPU_HIGH_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            requestid = pEngine->sendReq(FUNC_CANCEL_GPU_HIGH_FREQ, NULL, 0, tid, timestamp);
        }
        pdbg("cancelGpuHighFreq ret:%d, tid:%d, timestamp:%d", requestid, tid, TOINT(timestamp / 1000000L));
        return requestid;
    }

    int64_t requestCpuCoreForThread(int scene, int64_t action, int *bindtids , int bindlen, int timeoutms, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CPU_CORE_FOR_THREAD);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "scene", scene);
            cJSON_AddNumberToObject(jsonRequest, "action", action);

            if (bindlen > 0 && bindtids != NULL) {
                cJSON *needBindTids;
                cJSON_AddItemToObject(jsonRequest, "bindtids", needBindTids = cJSON_CreateArray());
                for (int index = 0; index < bindlen; index++) {
                    cJSON_AddItemToArray(needBindTids, cJSON_CreateNumber(bindtids[index]));
                }
            }

            cJSON_AddNumberToObject(jsonRequest, "timeoutMs", timeoutms);


            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CPU_CORE_FOR_THREAD, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::RequestCPUCoreForThread request;
            request.set_scene(scene);
            for (int index = 0; index < bindlen; index++) {
                request.add_bindtids(bindtids[index]);
            }
            request.set_timeoutms(timeoutms);
            request.set_action(action);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_CPU_CORE_FOR_THREAD, body, len, tid, timestamp);
        }
        pdbg("requestCpuCoreForThread requestid:%lld, scene:%d, action:%d, bindlen:%d, timeoutms:%d", requestid, scene, TOINT(action), bindlen, timeoutms);
        return requestid;
    }

    int64_t cancelCpuCoreForThread(int *unbindtids , int unbindlen, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        if (unbindlen <= 0 || unbindtids == NULL) {
            perr("cancelCpuCoreForThread unbindlen :%d", unbindlen);
            return -2;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CANCEL_CPU_CORE_FOR_THREAD);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);

            if (unbindlen > 0 && unbindtids != NULL) {
                cJSON *needUnBindTids;
                cJSON_AddItemToObject(jsonRequest, "unbindtids", needUnBindTids = cJSON_CreateArray());
                for (int index = 0; index < unbindlen; index++) {
                    cJSON_AddItemToArray(needUnBindTids, cJSON_CreateNumber(unbindtids[index]));
                }
            }

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CANCEL_CPU_CORE_FOR_THREAD, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::CancelCPUCoreForThread request;
            for (int index = 0; index < unbindlen; index++) {
                request.add_bindtids(unbindtids[index]);
            }
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_CANCEL_CPU_CORE_FOR_THREAD, body, len, tid, timestamp);
        }
        pdbg("cancelCpuCoreForThread requestid:%lld, unbindlen:%d", requestid, unbindlen);
        return requestid;
    }

    int64_t requestHighIOFreq(int scene, int64_t action, int level, int timeoutms, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }
        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_HIGH_IO_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "scene", scene);
            cJSON_AddNumberToObject(jsonRequest, "action", action);
            cJSON_AddNumberToObject(jsonRequest, "iolevel", level);
            cJSON_AddNumberToObject(jsonRequest, "timeoutMs", timeoutms);


            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_HIGH_IO_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::RequestHighIOFreq request;
            request.set_scene(scene);
            request.set_level(level);
            request.set_timeoutms(timeoutms);
            request.set_action(action);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_HIGH_IO_FREQ, body, len, tid, timestamp);
        }
        pdbg("requestCpuCoreForThread requestid:%lld, scene:%d, action:%d, level:%d, timeoutms:%d", requestid, scene, TOINT(action), level, timeoutms);
        return requestid;

    }

    int64_t cancelHighIOFreq(int tid, uint64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }
        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CANCEL_HIGH_IO_FREQ);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);
            cJSON_AddNumberToObject(jsonRequest, "iolevel", IO_LEVEL_0);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CANCEL_HIGH_IO_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            requestid = pEngine->sendReq(FUNC_CANCEL_HIGH_IO_FREQ, NULL, 0, tid, timestamp);
        }
        pdbg("cancelHighIOFreq requestid:%lld", requestid);
        return requestid;
    }

    int64_t requestScreenResolution(int level, std::string uiName, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_SET_SCREEN_RESOLUTION);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddItemToObject(jsonRequest, "uiName", cJSON_CreateString(uiName.c_str()));
            cJSON_AddNumberToObject(jsonRequest, "level", level);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_SET_SCREEN_RESOLUTION, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RequestScreenResolution request;
            request.set_level(level);
            request.set_uiname(uiName);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_SET_SCREEN_RESOLUTION, body, len, tid, timestamp);
        }
        pdbg("requestScreenResolution requestid:%lld, level:%d, uiName:%s", requestid, level, uiName.c_str());
        return requestid;
    }

    int64_t resetScreenResolution(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_RESET_SCREEN_RESOLUTION);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_RESET_SCREEN_RESOLUTION, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            requestid = pEngine->sendReq(FUNC_RESET_SCREEN_RESOLUTION, NULL, 0, tid, timestamp);
        }
        pdbg("resetScreenResolution requestid:%lld ", requestid);
        return requestid;
    }

    int64_t registerANRCallback(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_REG_ANR_CALLBACK);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);
            cJSON_AddNumberToObject(jsonRequest, "type", REGISTER_CALLBACK_TYPE_ANR);
            cJSON_AddNumberToObject(jsonRequest, "uid", getuid());
            cJSON_AddNumberToObject(jsonRequest, "pid", getpid());

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_REG_ANR_CALLBACK, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RegisterCallback request;
            request.set_type(REGISTER_CALLBACK_TYPE_ANR);
            request.set_uid(getuid());
            request.set_pid(getpid());
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_REG_ANR_CALLBACK, body, len, tid, timestamp);
        }
        pdbg("registerANRCallback requestid:%lld ", requestid);
        return requestid;
    }

    int64_t registerSystemEventCallback(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_REG_SYSTEM_EVENT_CALLBACK);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);
            cJSON_AddNumberToObject(jsonRequest, "type", REGISTER_CALLBACK_TYPE_SYSTEM_EVENT);
            cJSON_AddNumberToObject(jsonRequest, "uid", getuid());
            cJSON_AddNumberToObject(jsonRequest, "pid", getpid());

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_REG_SYSTEM_EVENT_CALLBACK, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RegisterCallback request;
            request.set_type(REGISTER_CALLBACK_TYPE_SYSTEM_EVENT);
            request.set_uid(getuid());
            request.set_pid(getpid());
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_REG_SYSTEM_EVENT_CALLBACK, body, len, tid, timestamp);
        }
        pdbg("registerSystemEventCallback requestid:%lld ", requestid);
        return requestid;
    }

    int64_t registerBootPreloadResource(std::vector<std::string> files, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_REG_PRELOAD_BOOT_RESOURCE);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);
            if (files.size() > 0) {
                cJSON *filelist;
                cJSON_AddItemToObject(jsonRequest, "filelist", filelist = cJSON_CreateArray());
                for (int index = 0; index < files.size(); index++) {
                    cJSON_AddItemToArray(filelist, cJSON_CreateString(files[index].c_str()));
                }
            }

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_REG_PRELOAD_BOOT_RESOURCE, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RequestBootPreLoadResource request;
            int count = files.size();
            for (int index = 0; index < count; index++) {
                request.add_filelist(files[index]);
            }
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_REG_PRELOAD_BOOT_RESOURCE, body, len, tid, timestamp);
        }
        pdbg("registerBootPreloadResource requestid:%lld, files size:%d", requestid, TOINT(files.size()));
        return requestid;
    }

    int64_t terminateApp(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_TERMINATE_APP);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_TERMINATE_APP, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            requestid = pEngine->sendReq(FUNC_TERMINATE_APP, NULL, 0, tid, timestamp);
        }
        pdbg("terminateApp requestid:%lld", requestid);
        return requestid;
    }

    int64_t requestUnifyCpuIOThreadCoreGpu(int scene, int64_t action, int cpulevel, int iolevel, int *bindtids , int gpulevel,  int bindlen, int timeoutms, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_UNIFY_CPU_IO_THREAD_CORE_GPU);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "scene", scene);
            cJSON_AddNumberToObject(jsonRequest, "action", action);
            cJSON_AddNumberToObject(jsonRequest, "cpulevel", cpulevel);
            cJSON_AddNumberToObject(jsonRequest, "iolevel", iolevel);
            cJSON_AddNumberToObject(jsonRequest, "gpulevel", gpulevel);

            if (bindlen > 0 && bindtids != NULL) {
                cJSON *needBindTids;
                cJSON_AddItemToObject(jsonRequest, "bindtids", needBindTids = cJSON_CreateArray());
                for (int index = 0; index < bindlen; index++) {
                    cJSON_AddItemToArray(needBindTids, cJSON_CreateNumber(bindtids[index]));
                }
            }

            cJSON_AddNumberToObject(jsonRequest, "timeoutMs", timeoutms);


            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_UNIFY_CPU_IO_THREAD_CORE_GPU, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::RequestUnifyCPUIOThreadCore request;
            request.set_scene(scene);
            request.set_action(action);
            request.set_cpulevel(cpulevel);
            request.set_iolevel(iolevel);
            request.set_gpulevel(gpulevel);
            if (bindlen > 0 && bindtids != NULL) {
                for (int index = 0; index < bindlen; index++) {
                    request.add_bindtids(bindtids[index]);
                    pdbg("requestUnifyCpuIOThreadCoreGpu bindtid:%d, index:%d", bindtids[index], index);
                }
            }
            request.set_timeoutms(timeoutms);

            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine ->sendReq(FUNC_UNIFY_CPU_IO_THREAD_CORE_GPU, body, len, tid, timestamp);
        }

        pdbg("requestUnifyCpuIOThreadCoreGpu requestid:%lld, scene:%d, action:%d, cpulevel:%d, iolevel:%d, bindlen:%d, gpulevel:%d, timeoutms:%d", requestid, scene, TOINT(action), cpulevel, iolevel, bindlen, gpulevel, timeoutms);
        return requestid;
    }

    int64_t cancelUnifyCpuIOThreadCoreGpu(int cancelcpu, int cancelio, int cancelthread, int *unbindtids , int unbindlen, int cancelgpu, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE_GPU);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_RESET);

            cJSON_AddNumberToObject(jsonRequest, "cpulevel", cancelcpu);
            cJSON_AddNumberToObject(jsonRequest, "iolevel", cancelio);
            cJSON_AddNumberToObject(jsonRequest, "gpulevel", cancelgpu);
            cJSON_AddNumberToObject(jsonRequest, "cancelthread", cancelthread);

            if (unbindlen > 0 && unbindtids != NULL) {
                cJSON *needUnBindTids;
                cJSON_AddItemToObject(jsonRequest, "unbindtids", needUnBindTids = cJSON_CreateArray());
                for (int index = 0; index < unbindlen; index++) {
                    cJSON_AddItemToArray(needUnBindTids, cJSON_CreateNumber(unbindtids[index]));
                }
            }

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE_GPU, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::CancelUnifyCPUIOThreadCore request;
            request.set_cancelcpu(cancelcpu);
            request.set_cancelio(cancelio);
            request.set_cancelthread(cancelthread);
            request.set_cancelgpu(cancelgpu);
            if (unbindlen > 0 && unbindtids != NULL) {
                for (int index = 0; index < unbindlen; index++) {
                    request.add_bindtids(unbindtids[index]);
                }
            }
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_CANCEL_UNIFY_CPU_IO_THREAD_CORE_GPU, body, len, tid, timestamp);
        }
        pdbg("cancelUnifyCpuIOThreadCoreGpu requestid:%lld, cancelcpu:%d, cancelio:%d, cancelthread:%d, unbindlen:%d, cancelgpu:%d ", requestid, cancelcpu, cancelio, cancelthread, unbindlen, cancelgpu);
        return requestid;
    }

    int64_t configure(const char *data, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }
        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_Parse(data);
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_CONFIGURE);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            pdbg("configure:%s", body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CONFIGURE, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::Configuration  request;
            request.set_data(data);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_CONFIGURE, body, len, tid, timestamp);

            pdbg("configure requestid:%lld, request size:%d", requestid, request.ByteSize());
        }
        return requestid;
    }

    int64_t getParameters(const char *data, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }
        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_Parse(data);
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_GET_PARAMETERS);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            pdbg("getParameters:%s", body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_GET_PARAMETERS, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::GetParameters  request;
            request.set_data(data);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_GET_PARAMETERS, body, len, tid, timestamp);

            pdbg("getParameters requestid:%lld, request size:%d", requestid, request.ByteSize());
        }
        return requestid;
    }

    int onCallback(int uid, int funcid, uint64_t requestid, int bodyFormat, uint64_t timestamp, int retCode, uint8_t *data, int len) {
        UNUSED(uid);

        pdbg("onCallback c2JavaCallback:0x%x, funcid:%d, requestid:%lld, bodyFormat:%d, retCode:%d, data:0x%x, len:%d", TOINT(c2JavaCallback()), funcid, requestid, bodyFormat, retCode, TOINT(data), len);
        if (!c2JavaCallback()) {
            return 0;
        }

        if (funcid == 0 && retCode) {//connect status
            c2JavaCallback()->callback(CALLBACK_TYPE_STATUS, requestid, timestamp, retCode, funcid, 0, NULL, 0);
            return 0;
        }

        if (len <= 0 || (!data)) {//no data, callback retCode
            c2JavaCallback()->callback(CALLBACK_TYPE_STATUS, requestid, timestamp, retCode, funcid, 0, NULL, 0);
            return 0;
        }

        if(bodyFormat == HEADER_BODYFORMAT_PROTO) {

            amc::DataCallback p;
            int ret = p.ParseFromArray(data, len);
            if (!ret) {
                pdbg("onCallback ParseFromArray ret:%d, c2JavaCallback:0x%x, funcid:%d, data:0x%x, len:%d",
                     ret, TOINT(c2JavaCallback()), funcid, TOINT(data), len);
                return 0;
            }

            c2JavaCallback()->callback(CALLBACK_TYPE_DATA, requestid, 0, 0, funcid, p.type(),
                                       (uint8_t *) p.data().data(),p.data().size());

        } else if(bodyFormat == HEADER_BODYFORMAT_JSON){
            c2JavaCallback()->callback(CALLBACK_TYPE_DATA, requestid, 0, 0, funcid, 0,
                                       data, len);
        }
        return 0;
    }

    int64_t registerCpuLoadCallback(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_REG_CPU_LOAD_CALLBACK);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);
            cJSON_AddNumberToObject(jsonRequest, "type", REGISTER_CALLBACK_TYPE_CPU_LOAD);
            cJSON_AddNumberToObject(jsonRequest, "uid", getuid());
            cJSON_AddNumberToObject(jsonRequest, "pid", getpid());

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_REG_CPU_LOAD_CALLBACK, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RegisterCallback request;
            request.set_type(REGISTER_CALLBACK_TYPE_CPU_LOAD);
            request.set_uid(getuid());
            request.set_pid(getpid());
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_REG_CPU_LOAD_CALLBACK, body, len, tid, timestamp);
        }
        pdbg("registerCpuLoadCallback requestid:%lld ", requestid);
        return requestid;
    }

    int64_t registerDeviceTemperatureCallback(int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_REG_DEVICE_TEMPERATURE_CALLBACK);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_NONE);
            cJSON_AddNumberToObject(jsonRequest, "type", REGISTER_CALLBACK_TYPE_DEVICE_TEMPERATURE);
            cJSON_AddNumberToObject(jsonRequest, "uid", getuid());
            cJSON_AddNumberToObject(jsonRequest, "pid", getpid());

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_REG_DEVICE_TEMPERATURE_CALLBACK, (uint8_t *)body, len, tid, timestamp);
            free(body);
        } else {
            amc::RegisterCallback request;
            request.set_type(REGISTER_CALLBACK_TYPE_DEVICE_TEMPERATURE);
            request.set_uid(getuid());
            request.set_pid(getpid());
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);
            requestid = pEngine->sendReq(FUNC_REG_DEVICE_TEMPERATURE_CALLBACK, body, len, tid, timestamp);
        }
        pdbg("registerDeviceTemperatureCallback requestid:%lld ", requestid);
        return requestid;
    }

    int64_t getCurrentTemperaturesWithType(int type, int tid, int64_t timestamp) {
        IClientEngine *pEngine = engine();
        if (!pEngine) {
            return -3;
        }

        int64_t requestid = 0L;
        if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
            cJSON *jsonRequest;
            jsonRequest = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonRequest, "jsonVersion", JSON_VERSION);
            cJSON_AddNumberToObject(jsonRequest, "funcid", FUNC_GET_CURRENT_TEMPERATURES_WITH_TYPE);
            cJSON_AddNumberToObject(jsonRequest, "status", STATUS_REQUEST);
            cJSON_AddNumberToObject(jsonRequest, "type", type);

            char *body = cJSON_Print(jsonRequest);
            std::string str(body);
            uint32_t len = str.length();
            cJSON_Delete(jsonRequest);
            requestid = pEngine ->sendReq(FUNC_CPU_HIGH_FREQ, (uint8_t *)body, len, tid, timestamp);
            free(body);

        } else {
            amc::GetCurrentTemperaturesWithType request;
            request.set_type(type);
            uint32_t len = request.ByteSize();
            uint8_t body[len];
            request.SerializeToArray(body, len);

            requestid = pEngine->sendReq(FUNC_CPU_HIGH_FREQ, body, len, tid, timestamp);
        }

        pdbg("getCurrentTemperaturesWithType requestid:%lld, type:%d, tid:%d, timestamp:%d" ,
             requestid, type, tid, TOINT(timestamp/1000000L));
        return requestid;
    }
};


#endif //AMC_PROTOCOL_H

