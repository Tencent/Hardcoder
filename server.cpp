/**
 * server function implement
 */
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <algorithm>

#include "server.h"
#include "amc.pb.h"

using namespace amc;

const std::string testANRMessage = "this is an anr test log message from server!";
const int testSystemEventCode = 12345;


class ManufacturerCoder : public HardCoder {
public:

    int getUidByAddress(const char *ip, int port) {
        UNUSED(ip);
        return LocalPortCheck::getUidByPort(LocalPortCheck::UDP_PORT, port);
    }

    bool checkPermission(std::vector<std::string> manufactures, std::vector<std::string> certs, int funcid, int uid, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder checkPermission manufactures:%d certs:%d funcid:%d uid:%d callertid:%d timestamp:%d", TOINT(manufactures.size()), TOINT(certs.size()), funcid, uid, callertid, TOINT(timestamp/1000000L));
        return true;
    }

    int requestCpuHighFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestCpuHighFreq scene:%d action:%d level:%d timeoutms:%d callertid:%d timestamp:%d", scene, TOINT(action), level, timeoutms, callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int cancelCpuHighFreq(int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder cancelCpuHighFreq callertid:%d timestamp:%d", callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int requestGpuHighFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestGpuHighFreq scene:%d action:%d level:%d timeoutms:%d callertid:%d timestamp:%d", scene, TOINT(action), level, timeoutms, callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int cancelGpuHighFreq(int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder cancelGpuHighFreq callertid:%d timestamp:%d", callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int requestCpuCoreForThread(int scene, int64_t action, std::vector<int>bindtids, int timeoutms, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestCpuCoreForThread scene:%d action:%d bindtids len:%d timeoutms:%d callertid:%d timestamp:%d", scene, TOINT(action), TOINT(bindtids.size()), timeoutms, callertid, TOINT(timestamp/1000000L));
        int len = bindtids.size();
        for (int index = 0; index < len; index++) {
            pdbg("ManufacturerCoder requestCpuCoreForThread bindtids index=%d tid=%d", index, bindtids[index]);
        }
        return RET_OK;
    }

    int cancelCpuCoreForThread(std::vector<int>bindtids, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder cancelCpuCoreForThread bindtids len:%d callertid:%d timestamp:%d", TOINT(bindtids.size()), callertid, TOINT(timestamp/1000000L));
        int len = bindtids.size();
        for (int index = 0; index < len; index++) {
            pdbg("ManufacturerCoder cancelCpuCoreForThread bindtids index=%d tid=%d", index, bindtids[index]);
        }
        return RET_OK;
    }

    int requestHighIOFreq(int scene, int64_t action, int level, int timeoutms, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestHighIOFreq scene:%d, action:%d, level:%d, timeoutms:%d, callertid:%d timestamp:%d", scene, TOINT(action), level, timeoutms, callertid, TOINT(timestamp/1000000L));
        return ERR_FUNCTION_NOT_SUPPORT;
    }

    int cancelHighIOFreq(int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder cancelHighIOFreq callertid:%d timestamp:%d", callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int requestScreenResolution(int level, std::string uiName, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestScreenResolution level::%d uiName:%s callertid:%d timestamp:%d", level, uiName.c_str(), callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int resetScreenResolution(int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder resetScreenResolution callertid:%d timestamp:%d", callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int terminateApp(int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder terminateApp callertid:%d timestamp:%d", callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int requestUnifyCpuIOThreadCoreGpu(int scene, int64_t action, int cpulevel, int iolevel, std::vector<int>bindtids, int gpulevel, int timeoutms, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder requestUnifyCpuIOThreadCoreGpu scene:%d action:%d cpulevel:%d iolevel:%d bindtids len:%d gpulevel:%d timeoutms:%d callertid:%d timestamp:%d",
            scene, TOINT(action), cpulevel,iolevel, TOINT(bindtids.size()), gpulevel, timeoutms, callertid, TOINT(timestamp/1000000L));
        int len = bindtids.size();
        for (int index = 0; index < len; index++) {
            pdbg("ManufacturerCoder requestUnifyCpuIOThreadCoreGpu bindtids index=%d tid=%d", index, bindtids[index]);
        }
        return RET_OK;
    }

    int cancelUnifyCpuIOThreadCoreGpu(int cancelcpu, int cancelio, int cancelthread, std::vector<int>bindtids, int cancelgpu, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder cancelUnifyCpuIOThreadCoreGpu cancelcpu:%d cancelio:%d cancelthread:%d bindtids len:%d cancelgpu:%d callertid:%d timestamp:%d",
                    cancelcpu, cancelio, cancelthread, TOINT(bindtids.size()), cancelgpu, callertid, TOINT(timestamp/1000000L));
        int len = bindtids.size();
        for (int index = 0; index < len; index++) {
            pdbg("ManufacturerCoder cancelUnifyCpuIOThreadCoreGpu bindtids index=%d tid=%d", index, bindtids[index]);
        }
        return RET_OK;
    }

    int configure(const std::string& data, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder configure");
        return RET_OK;
    }

    int getParameters(const std::string& data, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder getParameters");
        return RET_OK;
    }

    int registerAnrCallback(int uid, int callertid, int64_t timestamp) {
        std::list<int>& cbL = this->anrCallbackUidList;
        pdbg("ManufacturerCoder registerAnrCallback anrCallbackList size:%d callertid:%d timestamp:%d", TOINT(cbL.size()), callertid, TOINT(timestamp/1000000L));

        if (!(cbL.empty()) && (std::find(cbL.begin(), cbL.end(), uid) !=  cbL.end())) {
            pdbg("ManufacturerCoder has registered anr before uid:%d", uid)
        } else {
            this->anrCallbackUidList.push_back(uid);
            pdbg("ManufacturerCoder registerAnrCallback uid:%d", uid);
        }

        // for test
//        pthread_t tid;
//        pthread_create(&tid, NULL, testANR, this);

        return RET_OK;
    }

    int registerSystemEventCallback(int uid, int callertid, int64_t timestamp) {
        std::list<int>& cbL = this->systemEventCallbackList;
        pdbg("ManufacturerCoder registerSystemEventCallback systemEventCallbackList size:%d callertid:%d timestamp:%d", TOINT(cbL.size()), callertid, TOINT(timestamp/1000000L));

        if (!(cbL.empty()) && (find(cbL.begin(), cbL.end(), uid) !=  cbL.end())) {
            pdbg("ManufacturerCoder has registered system event callback before uid:%d", uid)
        } else {
            this->systemEventCallbackList.push_back(uid);
            pdbg("ManufacturerCoder registerSystemEventCallback uid:%d", uid);
        }

        // for test
//        pthread_t tid;
//        pthread_create(&tid, NULL, testANR, this);

        return RET_OK;
    }

    int registerCpuLoadCallback(int uid, int callertid, int64_t timestamp) {
        std::list<int>& cbL = this->cpuLoadCallbackUidList;
        pdbg("ManufacturerCoder registerCpuLoadCallback cpuLoadCallbackUidList size:%d callertid:%d timestamp:%d", TOINT(cbL.size()), callertid, TOINT(timestamp/1000000L));

        if (!(cbL.empty()) && (std::find(cbL.begin(), cbL.end(), uid) !=  cbL.end())) {
            pdbg("ManufacturerCoder has registered cpuload before uid:%d", uid)
        } else {
            this->cpuLoadCallbackUidList.push_back(uid);
            pdbg("ManufacturerCoder registerCpuLoadCallback uid:%d", uid);
        }

        // for test
//        pthread_t tid;
//        pthread_create(&tid, NULL, testANR, this);

        return RET_OK;
    }

    int registerDeviceTemperatureCallback(int uid, int callertid, int64_t timestamp) {
        std::list<int>& cbL = this->deviceTemperatureCallbackUidList;
        pdbg("ManufacturerCoder registerDeviceTemperatureCallback deviceTemperatureCallbackUidList size:%d callertid:%d timestamp:%d", TOINT(cbL.size()), callertid, TOINT(timestamp/1000000L));

        if (!(cbL.empty()) && (std::find(cbL.begin(), cbL.end(), uid) !=  cbL.end())) {
            pdbg("ManufacturerCoder has registered device temperature before uid:%d", uid)
        } else {
            this->deviceTemperatureCallbackUidList.push_back(uid);
            pdbg("ManufacturerCoder registerDeviceTemperatureCallback uid:%d", uid);
        }

        // for test
//        pthread_t tid;
//        pthread_create(&tid, NULL, testANR, this);

        return RET_OK;
    }

    int getCurrentTemperaturesWithType(int type, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder getCurrentTemperaturesWithType type:%d callertid:%d timestamp:%d", type, callertid, TOINT(timestamp/1000000L));
        return RET_OK;
    }

    int registerBootPreloadResource(std::vector<std::string> file, int callertid, int64_t timestamp) {
        pdbg("ManufacturerCoder registerBootPreloadResource file size:%d callertid:%d timestamp:%d", TOINT(file.size()), callertid, TOINT(timestamp));
        return RET_OK;
    }

    // to be invoked by system on event
    void onEvent(int eventCode) {
        pdbg("callbackThread event:%d", eventCode);
        amc::DataCallback  dataCallback;
        dataCallback.set_type(REGISTER_CALLBACK_TYPE_SYSTEM_EVENT);
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", eventCode);
        buffer[sizeof(buffer)-1] = '\0';
        dataCallback.set_data(buffer);
        cbQueue.push(dataCallback);
    }

    // to be invoked by system on anr
    void onANR(std::string log) {
        pdbg("callbackThread log:%s", log.c_str());
        amc::DataCallback  dataCallback;
        dataCallback.set_type(REGISTER_CALLBACK_TYPE_ANR);
        dataCallback.set_data(log);
        cbQueue.push(dataCallback);
    }


    explicit ManufacturerCoder(IHardCoderDataCallback *callback) {
        this->callback = callback;
        pthread_t tid;
        pthread_create(&tid, NULL, callbackThread, this);
    }

    ~ManufacturerCoder() {
        this->anrCallbackUidList.clear();
        this->systemEventCallbackList.clear();
        this->cpuLoadCallbackUidList.clear();
        this->deviceTemperatureCallbackUidList.clear();
        callback = NULL;
    }

    static void* callbackThread(void *args) {
        pdbg("callbackThread run pid:%d tid:%d", getpid(), gettid());
        ManufacturerCoder *coder = reinterpret_cast<ManufacturerCoder *>(args);
        while (1) {
            IHardCoderDataCallback *callback = coder->callback;
            amc::DataCallback  dataCallback = coder->cbQueue.pop();
            int type = dataCallback.type();

            if (type == REGISTER_CALLBACK_TYPE_SYSTEM_EVENT) {
                std::list<int> cbl = coder->systemEventCallbackList;
                if (cbl.empty()) {
                    pdbg("systemEventCallbackList is empty size:%d continue", TOINT(cbl.size()));
                    continue;
                }
                std::list<int>::iterator it;
                for (it = cbl.begin(); it != cbl.end(); ++it) {
                    int len = dataCallback.ByteSize();
                    uint8_t body[len] ;
                    dataCallback.SerializeToArray(body,len);
                    callback->onCallback(*it, FUNC_REG_SYSTEM_EVENT_CALLBACK, 0, 0, 0, 0 , body, len);
                    pdbg("system event callback done for succ uid:%d", *it);
                }

            } else if (type == REGISTER_CALLBACK_TYPE_ANR) {
                std::list<int> cbl = coder->anrCallbackUidList;
                if (cbl.empty()) {
                    pdbg("anrCallbackUidList is empty size:%d continue", TOINT(cbl.size()));
                    continue;
                }
                std::list<int>::iterator it;
                for (it = cbl.begin(); it != cbl.end(); ++it) {
                    int len = dataCallback.ByteSize();
                    uint8_t body[len];
                    dataCallback.SerializeToArray(body, len);
                    callback->onCallback(*it, FUNC_REG_ANR_CALLBACK, 0, 0, 0, 0, body, len);
                    pdbg("anr callback done for succ uid:%d", *it);
                }
            } else if (type == REGISTER_CALLBACK_TYPE_CPU_LOAD) {
                std::list<int> cbl = coder->cpuLoadCallbackUidList;
                if (cbl.empty()) {
                    pdbg("cpuLoadCallbackUidList is empty size:%d continue", TOINT(cbl.size()));
                    continue;
                }
                std::list<int>::iterator it;
                for (it = cbl.begin(); it != cbl.end(); ++it) {
                    int len = dataCallback.ByteSize();
                    uint8_t body[len];
                    dataCallback.SerializeToArray(body, len);
                    callback->onCallback(*it, FUNC_REG_CPU_LOAD_CALLBACK, 0, 0, 0, 0, body, len);
                    pdbg("cpu load callback done for succ uid:%d", *it);
                }
            } else if (type == REGISTER_CALLBACK_TYPE_DEVICE_TEMPERATURE) {
                std::list<int> cbl = coder->deviceTemperatureCallbackUidList;
                if (cbl.empty()) {
                    pdbg("deviceTemperatureCallbackUidList is empty size:%d continue", TOINT(cbl.size()));
                    continue;
                }
                std::list<int>::iterator it;
                for (it = cbl.begin(); it != cbl.end(); ++it) {
                    int len = dataCallback.ByteSize();
                    uint8_t body[len];
                    dataCallback.SerializeToArray(body, len);
                    callback->onCallback(*it, FUNC_REG_DEVICE_TEMPERATURE_CALLBACK, 0, 0, 0, 0, body, len);
                    pdbg("device temperature callback done for succ uid:%d", *it);
                }
            }
        }
        pwrn("callbackThread loop QUIT now.");
        return NULL;
    }

    //for test system event
    static void* testSystemEvent(void *args) {
        sleep(10);
        ManufacturerCoder *coder = (ManufacturerCoder *) args;
        coder->onEvent(testSystemEventCode);
        /**
        IHardCoderDataCallback *callback = coder->callback;
        list<int>& cbL = coder->systemEventCallbackList;
        if (cbL.empty()) {
            pdbg("testSystemEvent but systemEventCallbackList is empty size:%d", cbL.size());
            return NULL;
        }

        amc::DataCallback  dataCallback;
        dataCallback.set_type(1);
        dataCallback.set_data(testSystemEvent);

        int len = dataCallback.ByteSize();
        uint8_t body[len];
        dataCallback.SerializeToArray(body,len);
        pdbg("testSystemEvent systemEventCallbackList size:%d %d", cbL.size());
        int64_t ret = callback->onCallback(cbL.front(), FUNC_REG_SYSTEM_EVENT_CALLBACK, body, len);
         **/
        pdbg("testSystemEvent exit tid:%d", gettid());
        return NULL;
    }

    // for test anr
    static void* testANR(void *args) {
        sleep(10);
        ManufacturerCoder* coder = (ManufacturerCoder *) args;
        coder->onANR(testANRMessage);
        /**
        IHardCoderDataCallback *callback = coder->callback;
        list<int>& cbL = coder->anrCallbackList;
        if (cbL.empty()) {
            pdbg("testANR but anrCallbackList is empty size:%d", cbL.size());
            return NULL;
        }

        amc::DataCallback  dataCallback;
        dataCallback.set_type(2);
        dataCallback.set_data(testANRMessage);

        int len = dataCallback.ByteSize();
        uint8_t body[len];
        dataCallback.SerializeToArray(body,len);
        pdbg("testANR anrCallbackList size:%d %d", cbL.size());
        int64_t ret = callback->onCallback(cbL.front(), FUNC_REG_ANR_CALLBACK, body, len);
         **/
        pdbg("testANR exit tid:%d", gettid());
        return NULL;
    }


private:
    std::list<int> anrCallbackUidList;
    std::list<int> cpuLoadCallbackUidList;
    std::list<int> deviceTemperatureCallbackUidList;
    std::list<int> systemEventCallbackList;
    IHardCoderDataCallback *callback;
    rqueue<amc::DataCallback, 0> cbQueue;

};

/**
 * for script test
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    setTag("HardCoderServer");
    if (argc == 2) {
        LocalSocketServer localsocket;
        ManufacturerCoder *hc = new ManufacturerCoder(static_cast<IHardCoderDataCallback *>(&localsocket));
        localsocket.start(argv[1], hc);
    } else if (argc > 2) {
         UdpServer udp;
         ManufacturerCoder *hc = new ManufacturerCoder(static_cast<IHardCoderDataCallback *>(&udp));
         udp.start(argv[1], atoi(argv[2]), hc);
    }
    else {
        fprintf(stderr, "usage:\n\t%s {ip} {port} (usage udp server)\nOR\n\t%s {path} (usage localsocket server)\n\n", argv[0], argv[0]);
    }
    return 0;
}



