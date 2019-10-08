/**
 * Hardcoder server sample implementation
 */
#ifndef HARDCODER_SERVER_H_H
#define HARDCODER_SERVER_H_H

#include <map>

#include "localsocket.h"
#include "udp.h"
#include "protocol.h"

class LocalSocketServer : public LocalSocket, public IHardCoderDataCallback {
private :
    rqueue<ServerDataWorker<int> *, 0> respQueue;
    HardCoder *inst;
    int running;

    typedef struct RecvCache {
        uint8_t reqHeaderBuf[sizeof(AMCReqHeader)];
        uint32_t reqHeaderOffset;
        uint8_t *reqPayload;
        uint32_t reqPayloadOffset;

        RecvCache() {
            reqHeaderOffset = 0;
            reqPayload = NULL;
            reqPayloadOffset = 0;
        }
        ~RecvCache() {
            reqHeaderOffset = 0;
            if (reqPayload) {
                delete[] reqPayload;
            }
            reqPayload = NULL;
            reqPayloadOffset = 0;
        }

    } RecvCache;

    std::map<int, RecvCache *> mapRecvCache;

    int recvEvent(Event event, int fd, int pid, uid_t uid, const char *path, uint8_t *recvData, int recvDataLen) {
        if (event == EVENT_ERROR) {
            perr("LocalSocketServer EVENT_ERROR: uid:%d path:%s", uid, path);
            return 0;
        }
        if (event == EVENT_CONNECT) {
            pwrn("Connect By uid:%d  fd:%d" , uid, fd);
            return 0;
        }

        if (event == EVENT_DATA) {

            uint8_t *readPtr = recvData;
            while (const uint32_t readLeft = (recvDataLen - (readPtr - recvData))) {
                if (!mapRecvCache.count(fd)) {
                    mapRecvCache[fd] = new RecvCache();
                }
                RecvCache * const recvCache = mapRecvCache[fd];

                if (recvCache->reqHeaderOffset < sizeof(AMCReqHeader)) {  // fill header

                    const int shouldRead = MIN(sizeof(AMCReqHeader) - recvCache->reqHeaderOffset, readLeft );
                    memcpy(recvCache->reqHeaderBuf + recvCache->reqHeaderOffset, readPtr, shouldRead);

                    readPtr += shouldRead;
                    recvCache->reqHeaderOffset += shouldRead;

                    if (recvCache->reqHeaderOffset < sizeof(AMCReqHeader)) { // wait next event
                        break;
                    }
                }
                AMCReqHeader *pHeader = (AMCReqHeader *)(recvCache->reqHeaderBuf);
                if (pHeader->begin != HEADER_BEGIN) {
                    perr("Check Header BEGIN failed:0x%x server:0x%x", pHeader->begin, HEADER_BEGIN);
                    delete recvCache;
                    mapRecvCache.erase(fd);
                    return -1;
                }

                if (recvCache->reqPayloadOffset < pHeader->bodylen) {
                    if (recvCache->reqPayload == NULL) {
                        recvCache->reqPayload = new uint8_t[pHeader->bodylen];
                    }

                    const int shouldRead = std::min(pHeader->bodylen - recvCache->reqPayloadOffset, readLeft );
                    memcpy(recvCache->reqPayload + recvCache->reqPayloadOffset, readPtr, shouldRead);

                    readPtr += shouldRead;
                    recvCache->reqPayloadOffset += shouldRead;

                    if (recvCache->reqPayloadOffset < pHeader->bodylen) { // wait next event
                        break;
                    }
                }

                ServerDataWorker<int> *pp = new ServerDataWorker<int>();
                int64_t requestId = pp->processReceive(uid, &fd, pHeader, recvCache->reqPayload, pHeader->bodylen, inst);
                if (requestId < 0) {
                    delete pp;
                } else {
                    respQueue.push(pp);
                }
                delete recvCache;
                mapRecvCache.erase(fd);
            }
            return 0;
        }
        perr("LocalSocketServer EVENT:%d ", event);
        return 0;
    }

public:
    LocalSocketServer() {
        inst = NULL;
        running = 0;
    }

    static void* localServerRespThread(void *args) {
        pdbg("localServerRespThread run pid:%d tid:%d", getpid(), gettid());
        LocalSocketServer *server = (LocalSocketServer *) args;
        while (server->running) {
            ServerDataWorker<int> *pp = server->respQueue.pop();
            int ret = server->sendData(pp->getRemote(), pp->getResp(), pp->getResLen());
            delete pp;

            pdbg("processResp sendRet:%d tid:%d queueSize:%d", ret, gettid(), server->respQueue.size());
        }
        pwrn("processResp loop QUIT now.");
        return NULL;
    }

    int start(const char *path, HardCoder *hardCoder) {
        inst = hardCoder;
        running = 1;
        pthread_t tid;
        pthread_create(&tid, NULL, localServerRespThread, this);
        int ret = createSocket(path, "");
        return ret > 0 ? startLoop(1) : ret;
    }

    int onCallback(int uid, int funcid, uint64_t requestid, int bodyFormat, uint64_t timestamp, int retCode, uint8_t *data, int len) {
        pdbg("onCallback uid:%d funcid:%d, len:%d", uid, funcid, len);

        ServerDataWorker<int> *pp = new ServerDataWorker<int>();

        int fd = this->findFdByUid(uid);
        if (fd < 0) {
            delete pp;
            return  -1;
        }

        int64_t requestId = pp->processSend(funcid, &fd, data, len);
        pdbg("callback funcid:%d, len:%d requestId:%d", funcid, len, requestId);
        respQueue.push(pp);
        return requestId;
    }

};

class UdpServer : public UDP, public IHardCoderDataCallback{
private :
    rqueue<ServerDataWorker<sockaddr_in> *, 0> respQueue;
    HardCoder *inst;
    int running;

    int recvEvent(Event event, struct sockaddr_in *addr, uint64_t sessionid, uint8_t *data, int len) {
        if (event == EVENT_TIMEOUT) {
            perr("TIMEOUT: addr:%s", sockaddrToString(*addr).c_str());
            return 0;
        }
        if (event == EVENT_CLOSED) {
            running = 0;
            perr("EVENT_CLOSED.");
            return 0;
        }
        if (event == EVENT_SESSION_FINISH) {
            pdbg("session finish :%d", TOINT(sessionid));
            return 0;
        }

        if (event == EVENT_DATA) {
            ServerDataWorker<sockaddr_in> *pp = new ServerDataWorker<sockaddr_in>();

            char ip[32] = {0};
            unsigned char *bytes = (unsigned char *) &(addr->sin_addr);
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
            int port = ntohs(addr->sin_port);

            int uid = inst->getUidByAddress(ip, port);

            int64_t requestId = pp->processReceive(uid, addr, (AMCReqHeader *) data, data + sizeof(AMCReqHeader), len - sizeof(AMCReqHeader), inst);
            if (requestId < 0) {
                delete pp;
            } else {
                respQueue.push(pp);
            }
        }
        perr("ERR EVENT:%d ", event);
        return 0;
    }

public:

    UdpServer() {
        inst = NULL;
        running = 0;
    }

    static void* udpServerThread(void *args) {
        UdpServer *server = (UdpServer *) args;
        while (server->running) {

            ServerDataWorker<sockaddr_in> *pp = server->respQueue.pop();
            int ret = server->send(pp->getRemote(), pp->getResp(), pp->getResLen());
            delete pp;

            pdbg("processResp sendRet:%d tid:%d queueSize:%d", ret, gettid(), server->respQueue.size());
        }
        pwrn("processResp loop QUIT now.");
        return NULL;
    }


    int start(const char *ip, int port, HardCoder *hardCoder) {
        inst = hardCoder;
        running = 1;
        pthread_t tid;
        pthread_create(&tid, NULL, udpServerThread, this);
        int ret = createSocket(ip, port, 50);
        if (ret > 0) {
            return startLoop();
        }
        return ret;
    }

    int onCallback(int uid, int funcid, uint64_t requestid, int bodyFormat, uint64_t timestamp, int retCode, uint8_t *data, int len) {
        UNUSED(data);
        pdbg("callback uid:%d funcid:%d, len:%d", uid, funcid, len);
        return 0;
    }

};

#endif //HARDCODER_SERVER_H_H
