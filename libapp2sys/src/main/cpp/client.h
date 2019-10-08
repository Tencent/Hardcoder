/**
 * Hardcoder client implementation
 */
#include <stdatomic.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sstream>
#include <jni.h>


#include "localsocket.h"
#include "udp.h"
#include "protocol.h"
#include "c2java.h"
#include "header.h"

class UdpClient : public UDP , public IClientEngine {

private:
    int m_timeout;
    struct sockaddr_in m_addr;
    IHardCoderDataCallback *m_DataCallback;

    int m_runningFlag;

    int recvEvent(Event event, struct sockaddr_in *addr, uint64_t sessionid, uint8_t *data, int len) {
        UNUSED(addr);
        if (event == EVENT_TIMEOUT) {
            pdbg("TIMEOUT");
            return 0;
        }
        if (event == EVENT_CLOSED) {
            pdbg("CLOSE.");
            return 0;
        }

        if (event == EVENT_SESSION_FINISH) {
            pdbg("session finish:%lld", sessionid);
            return 0;
        }

        if (event == EVENT_DATA) {

            AMCRespHeader *pHead = ((AMCRespHeader *) data);
            if(m_DataCallback != NULL) {
                m_DataCallback->onCallback(0, pHead->funcid, pHead->requestid, 0, pHead->timestamp, pHead->retCode, data + sizeof(AMCRespHeader), len - sizeof(AMCRespHeader));
            }
            pdbg("pHead->requestid:%lld", pHead->requestid);
            return 0;
        }
        pdbg("ERR EVENT:%d", event);
        return -1;
    }

    static void* udpClientThread(void *args) {
        UdpClient *p = ((UdpClient *) args);
        int ret = p->startLoop();
        pdbg("socket:%d finish. tid:%d", ret, gettid());
        __sync_fetch_and_sub(&(p->m_runningFlag), 1);
        return NULL;
    }

public:
    UdpClient() {
        m_runningFlag = 0;
        m_timeout = 0;
        m_DataCallback = NULL;
        memset(&m_addr, 0, sizeof(m_addr));
    }

    bool isRunning() {
        return m_runningFlag;
    }

    int64_t sendReq(int funcid, uint8_t *data, int dataLen, int tid, int64_t timestamp) {
        if (!isRunning()) {
            return -1;
        }
        uint8_t *pack = NULL;
        uint32_t packLen = 0;
        int64_t id = genReqPack(funcid, data, dataLen, &pack, &packLen, tid, timestamp);
        if (id <= 0) {
            delete[] pack;
            return id;
        }
        send(m_addr, pack, packLen);
        delete[] pack;
        return id;
    }

    int start(const char *ip, const int port, const int timeout, IHardCoderDataCallback *callback) {
        m_addr = toSockaddr(ip, port);
        m_timeout = timeout;
        m_DataCallback = callback;

        int ret = createSocket("", 0, m_timeout);
        if (ret > 0) {
            __sync_fetch_and_add(&m_runningFlag, 1);
            pthread_t tid;
            pthread_create(&tid, NULL, udpClientThread, this);
        }
        return 0;
    }
};

class LocalsocketClient : public LocalSocket, public IClientEngine {

public:
    LocalsocketClient() {
        m_runningFlag = 0;
        m_DataCallback = NULL;
        respHeaderOffset = 0;
        respPayload = NULL;
        respPayloadOffset = 0;
    }

private:
    std::string m_remotepath;
    std::string m_localpath;

    IHardCoderDataCallback* m_DataCallback;

    uint8_t respHeaderBuf[sizeof(AMCRespHeaderV2)];
    uint32_t respHeaderOffset;
    uint8_t *respPayload;
    uint32_t respPayloadOffset;

    int m_runningFlag;

    int recvEvent(Event event, int fd, int pid, uid_t uid, const char *path, uint8_t *recvData, int recvDataLen) {

        if (event == EVENT_ERROR) {
            perr("LocalsocketClient recvEvent EVENT_ERROR: uid:%d, path:%s, fd:%d. Reconnect now.", uid, path, fd);
            return 0;
        }

        if (event == EVENT_DATA) {
            int recvDataOffset = 0;

            while (recvDataOffset < recvDataLen) {
                int recvDataLeft = recvDataLen - recvDataOffset;

                if (respHeaderOffset < sizeof(AMCRespHeader)) {  // fill header
                    int shouldRead = sizeof(AMCRespHeader) - respHeaderOffset;
                    shouldRead = shouldRead < recvDataLeft ? shouldRead : recvDataLeft;

                    memcpy(respHeaderBuf + respHeaderOffset, recvData + recvDataOffset, shouldRead);
                    recvDataOffset += shouldRead;
                    respHeaderOffset += shouldRead;
                }
                if (respHeaderOffset == sizeof(AMCRespHeader)) { // fill data
                    AMCRespHeader *pHeader = (AMCRespHeader *)(respHeaderBuf);
                    pdbg("pHead->requestid:%lld, version:%d, recvDataLen:%d", pHeader->requestid, pHeader->version, recvDataLen);

                    if (pHeader->begin != HEADER_BEGIN) {
                        perr("Check Header BEGIN failed:0x%x server:0x%x", pHeader->begin, HEADER_BEGIN);
                        if (respPayload) {
                            delete[] respPayload;
                        }
                        respPayload = NULL;
                        respHeaderOffset = 0;
                        return -1;
                    }

                    if(pHeader->version >= HEADER_JSON_VERSION){//json, use AMCRespHeaderV2
                        //fill header
                        recvDataLeft = recvDataLen - recvDataOffset;
                        int shouldRead = sizeof(AMCRespHeaderV2) - respHeaderOffset;
                        shouldRead = shouldRead < recvDataLeft ? shouldRead : recvDataLeft;
                        memcpy(respHeaderBuf + respHeaderOffset, recvData + recvDataOffset, shouldRead);
                        recvDataOffset += shouldRead;
                        respHeaderOffset += shouldRead;
                        AMCRespHeaderV2 *pHeaderV2 = (AMCRespHeaderV2 *)(respHeaderBuf);

                        if (pHeaderV2->bodylen) {
                            if (respPayload == NULL) {
                                respPayload = new uint8_t[pHeaderV2->bodylen];
                            }
                            pdbg("LocalsocketClient recvEvent EVENT_DATA, pHeaderV2->bodylen:%d, respPayloadOffset:%d", pHeaderV2->bodylen, respPayloadOffset);
                            int shouldRead = pHeaderV2->bodylen - respPayloadOffset;//respPayloadOffset = 0, shouldRead=bodylen
                            shouldRead = shouldRead < recvDataLeft ? shouldRead : recvDataLeft;

                            memcpy(respPayload + respPayloadOffset, recvData + recvDataOffset, shouldRead);
                            respPayloadOffset += shouldRead;//=bodylen
                            recvDataOffset += shouldRead;//datalen
                        }
                        if (respPayloadOffset == pHeaderV2->bodylen) {
//                            pdbg("LocalsocketClient recvEvent EVENT_DATA, funcid:%d, timestamp:%dms, headerlen:%d", pHeaderV2->funcid, TOINT(pHeaderV2->timestamp/1000000L), respHeaderOffset);
                            pdbg("LocalsocketClient recvEvent EVENT_DATA, funcid:%d, headerlen:%d, bodylen:%d", pHeaderV2->funcid, respHeaderOffset, respPayloadOffset);
                            if(m_DataCallback != NULL) {
                                m_DataCallback->onCallback(0, pHeaderV2->funcid, pHeaderV2->requestid, pHeaderV2->bodyformat, pHeaderV2->timestamp, pHeaderV2->retCode, respPayload, respPayloadOffset);
                            }
                            if (respPayload) {
                                delete[] respPayload;
                            }
                            respPayload  = NULL;
                            respHeaderOffset = 0;
                            respPayloadOffset = 0;
                        }
                    } else {//proto
                        if (pHeader->bodylen) {
                            if (respPayload == NULL) {
                                respPayload = new uint8_t[pHeader->bodylen];
                            }

                            int shouldRead = pHeader->bodylen - respPayloadOffset;//bodylen
                            shouldRead = shouldRead < recvDataLeft ? shouldRead : recvDataLeft;

                            memcpy(respPayload + respPayloadOffset, recvData + recvDataOffset, shouldRead);
                            respPayloadOffset += shouldRead;//bodylen
                            recvDataOffset += shouldRead;//datalen
                        }

                        if (respPayloadOffset == pHeader->bodylen) {
                            pdbg("LocalsocketClient recvEvent EVENT_DATA, funcid:%d, timestamp:%dms, bodylen:%d",
                                 pHeader->funcid, TOINT(pHeader->timestamp / 1000000L), respHeaderOffset);

                            if(m_DataCallback != NULL) {
                                m_DataCallback->onCallback(0, pHeader->funcid, pHeader->requestid, HEADER_BODYFORMAT_PROTO, pHeader->timestamp, pHeader->retCode, respPayload, respPayloadOffset);
                            }
                            if (respPayload) {
                                delete[] respPayload;
                            }
                            respPayload  = NULL;
                            respHeaderOffset = 0;
                            respPayloadOffset = 0;
                        }
                    }
                }
            }
            return 0;
        }
        pdbg("LocalsocketClient ERROR EVENT:%d", event);
        return -1;
    }

    static void* localsocketClientThread(void *args) {
        LocalsocketClient *p = ((LocalsocketClient *) args);
        p->startLoop(0);
        __sync_fetch_and_sub(&(p->m_runningFlag), 1);
        pwrn("localsocketClientThread finish. tid:%d", gettid());
        return NULL;
    }


public:
    bool isRunning() {
        return m_runningFlag;
    }

    int start(const char *remote, const char *local, IHardCoderDataCallback *callback) {
        if (isRunning()) {
            return 0;
        }

        respHeaderOffset = 0;
        respPayload = NULL;
        respPayloadOffset = 0;
        m_localpath = local;
        m_remotepath = remote;
        m_DataCallback = callback;

        uninit(1);
        int ret = createSocket(m_localpath.c_str(), m_remotepath.c_str());
        if (ret > 0) {
            __sync_fetch_and_add(&m_runningFlag, 1);
            pthread_t tid;
            pthread_create(&tid, NULL, localsocketClientThread, this);
        }
        return ret;
    }

    int64_t sendReq(int funcid, uint8_t *data, int dataLen, int tid, int64_t timestamp) {
        if (!isRunning()) {
            return -1;
        }
        uint8_t *pack = NULL;
        uint32_t packLen = 0;
        int64_t requestId = genReqPack(funcid, data, dataLen, &pack, &packLen, tid, timestamp);
        if (requestId <= 0) {
            perr("genReqPack failed, requestId:%lld", requestId);
            delete[] pack;
            return requestId;
        }
        clientSendData(pack, packLen);
        delete[] pack;
        return requestId;
    }

};


class Client : public ClientProtocal {

protected:
    IC2JavaCallback* c2JavaCallback() {
        return m_pC2JavaCallback;
    }

    IClientEngine* engine() {
        if (!m_enable) {
            return NULL;
        }
        if (m_pClientEngine && m_pClientEngine->isRunning()) {
            return m_pClientEngine;
        }
        pdbg("ClientEngine is not running, try Reconnect");

        if (m_pC2JavaCallback) {
            m_pC2JavaCallback->callback(CALLBACK_TYPE_STATUS, 0, 0, ERR_CLIENT_DISCONNECT, 0, 0, NULL, 0);
        }
        if (tryStartEngine() == 0) {
            return m_pClientEngine;
        }
        return NULL;
    }

private:
    IC2JavaCallback *m_pC2JavaCallback;
    IClientEngine *m_pClientEngine;
    int m_enable;

    int m_retryConnectInterval;

    std::string m_remote;
    int m_port;
    std::string m_local;

    int64_t m_lastConnect;

    int tryStartEngine() {
        if ((m_remote.length() == 0) || (m_port <= 0 && m_local.length() == 0)) { //param check
            return -1;
        }
        if (m_pClientEngine && m_pClientEngine->isRunning()) {  //running
            return -2;
        }

        int64_t curr = getMillisecond();

        pdbg("tryStartEngine, TimeDiff:%d, remote:%s, running:%d", TOINT(curr - m_lastConnect), m_remote.c_str(),
             m_pClientEngine ? m_pClientEngine->isRunning() : -1);

        if (curr - m_lastConnect < (int64_t)m_retryConnectInterval * 1000) {
            return -3;
        }
        m_lastConnect = curr;

        if (m_pClientEngine) { // disconnected
            delete m_pClientEngine;
            m_pClientEngine = NULL;
        }
        IClientEngine *pEngine = NULL;
        if (m_port != 0) {
            pEngine = new UdpClient();
            if (static_cast<UdpClient *>(pEngine)->start(m_remote.c_str(), m_port, 500, this) > 0 && pEngine->isRunning()) {
                m_pClientEngine = pEngine;
                return 0;
            } else {
                delete pEngine;
            }
        } else {
            pEngine = new LocalsocketClient();
            if (static_cast<LocalsocketClient *>(pEngine)->start(m_remote.c_str(), m_local.c_str(), this) > 0 && pEngine->isRunning()) {
                m_pClientEngine = pEngine;
                m_pC2JavaCallback->callback(CALLBACK_TYPE_STATUS, 0, 0, ERR_CLIENT_CONNECT, 0, 0, NULL, 0);
                return 0;
            } else {
                delete pEngine;
            }
        }
        return -3;
    }

public:
    Client() {
        m_enable = 1;  // DEFAULT is enabled
        std::string m_remote = "";
        m_port = 0;
        std::string m_local = "";
        m_retryConnectInterval = 30; // 30 sec
        m_lastConnect = 0;

        m_pClientEngine = NULL;
        m_pC2JavaCallback = NULL;
    }

    void setEnable(int enableFlag) {
        bool restrart = m_enable == 0 && enableFlag;
        if (restrart) {
            tryStartEngine();
        }
        m_enable = enableFlag;
        pdbg("setEnable %d to %d, restart:%d", m_enable, enableFlag, restrart);
    }

    void setRetryConnectInterval(int retryConnectInterval) {
        if (retryConnectInterval > 0) {
            m_retryConnectInterval = retryConnectInterval;
            pdbg("setRetryConnectInterval:%ds", m_retryConnectInterval);
        }
    }

    bool isEnable() {
        return m_enable;
    }

    bool isRunning() {
        return m_pClientEngine && m_pClientEngine->isRunning();
    }

    int init(const char *remote, const int port, const char *local, IC2JavaCallback *cb) {
        if (m_pClientEngine) {
            pdbg("ClientInit client already inited, remote:%s, port:%d, local:%s, cb:%d",
                 m_remote.c_str(), m_port, m_local.c_str(), TOINT(cb));
            m_pC2JavaCallback->callback(CALLBACK_TYPE_STATUS, 0, 0, ERR_CLIENT_ALREADY_INIT, 0, 0, NULL, 0);
            return 0;
        }

        m_pC2JavaCallback = cb;

        if(remote == NULL){
            m_remote = "";
        } else {
            char *c1 = getSocketName(remote, "|");
            if(c1 == NULL){
                m_remote = "";
            } else {
                m_remote = c1;
            }
            delete[] c1;
        }

        m_local = (local == NULL ? "" : local);
        m_port = port;

        char *version = getVersion(remote, "|");

        if (version == NULL) {
            HEADER_VERSION = HEADER_PROTOCAL_VERSION;;
        } else {
            HEADER_VERSION = atoi(version);
            if (HEADER_VERSION <= 0) {
                HEADER_VERSION = HEADER_PROTOCAL_VERSION;
            }
        }

        pdbg("init m_remote:%s, HEADER_VERSION:%d", m_remote.c_str(), HEADER_VERSION);
        tryStartEngine();
        delete[] version;
        return 0;
    }

};


