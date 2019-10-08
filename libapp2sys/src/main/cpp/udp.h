/**
 * UDP implementation
 */
#ifndef AMC_UDP_H
#define AMC_UDP_H

#include "util.h"
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

class UDP {

protected:

    typedef enum {
        EVENT_ERROR, EVENT_TIMEOUT, EVENT_CLOSED, EVENT_DATA, EVENT_SESSION_FINISH
    } Event;

    virtual int recvEvent(Event event, struct sockaddr_in *addr, uint64_t sessionid, uint8_t *data, int len) = 0;

private:

    const static int CONST_MAXRETRANS = 5;
    const static int CONST_WINDOW = 5;
//  const static int CONST_DATAQUEUE_SIZE = 10;
    const static int CONST_VERSION = 1;
    const static int CONST_MAXPKTSIZE = 1000;

    typedef enum {
        SYN_SENT, OPENING, OPEN, FIN_SENT
    } Status;

    static const char* getStatus(Status s) {
        if (s == SYN_SENT) return "SYN_SENT";
        if (s == OPENING) return "OPENING";
        if (s == OPEN) return "OPEN";
        if (s == FIN_SENT) return "FIN_SENT";
        return "ERR_STATE";
    }

    const static int PACKAGE_DATA = 1;
    const static int PACKAGE_ACK = 2;
    const static int PACKAGE_SYN = 4;
    const static int PACKAGE_FIN = 5;

    static const char* packageTypeStr(unsigned char t) {
        if (t == PACKAGE_DATA) return "DATA";
        if (t == PACKAGE_ACK) return "ACK";
        if (t == PACKAGE_SYN) return "SYN";
        if (t == PACKAGE_FIN) return "FIN";
        return "ERR_TYPE";
    }

    typedef struct UdpPkg {
        uint8_t version;
        uint8_t type;
        uint16_t datalen;
        uint32_t datacrc;
        uint32_t totallen;
        uint32_t offset;
        uint32_t seq;
        int64_t sessionid;
        uint8_t data[CONST_MAXPKTSIZE];

        UdpPkg() {
            version = 0;
            type = 0;
            datalen = 0;
            datacrc = 0;
            totallen = 0;
            offset = 0;
            seq = 0;
            sessionid = 0;
        }

        int pkgLen() {
            return datalen + sizeof(UdpPkg) - sizeof(data);
        }

        std::string toString() {
            char buf[512];
            sprintf(buf, "P[%x v:%d t:%d,%s l:%d dl:%d c:0x%x tl:%d of:%d s:%d sid:%d]", TOINT(this) , version, type, packageTypeStr(type),
                    pkgLen(), datalen, datacrc, totallen, offset, seq, TOINT(sessionid));
            return (std::string) buf;
        }

    }__attribute__ ((packed)) UdpPkg;


    typedef struct TimeoutEvent {
        int64_t timeout;
        int tryCount;
        struct sockaddr_in toaddr;
        UdpPkg *udpPkg;

        TimeoutEvent() {
            timeout = 0;
            tryCount = 0;
            udpPkg = NULL;
            memset(&toaddr, 0, sizeof(toaddr));
        }

        std::string toString() {
            char buf[512];
            sprintf(buf, "T[%x %s to:%d tc:%d %s]", TOINT(this) , sockaddrToString(toaddr).c_str(), TOINT(timeout - getMillisecond()),
                    tryCount,
                    udpPkg ? udpPkg->toString().c_str() : "pkg:NULL");
            return (std::string) buf;
        }
    } TimeoutEvent;


    typedef struct Sender {
        struct sockaddr_in toaddr;
        Status status;
        uint32_t seq;
        TimeoutEvent finTimeoutEvent;
        TimeoutEvent synTimeoutEvent;
        TimeoutEvent dataWindow[CONST_WINDOW];
        std::queue<UdpPkg *> dataQueue;

        std::string toString() {
            char buf[512];
            sprintf(buf, "S[%x %s %s s:%d q:%d syn:%s fin:%s]", TOINT(this) , sockaddrToString(toaddr).c_str(), getStatus(status), seq,
                    (int)dataQueue.size(), synTimeoutEvent.toString().c_str(), finTimeoutEvent.toString().c_str());
            return (std::string) buf;
        }
    } Sender;

    typedef struct Receiver {
        struct sockaddr_in fromaddr;
        Status status;
        uint32_t expectedSeq;
        std::map<int64_t, std::pair<uint32_t, uint8_t *> *> sessionMap;

        std::string toString() {
            char buf[512];
            sprintf(buf, "R[%x %s %s s:%d m:%d]", TOINT(this), sockaddrToString(fromaddr).c_str(), getStatus(status), expectedSeq,
                    (int)sessionMap.size());
            return (std::string) buf;
        }
    } Receiver;

    typedef struct SendPack {
        struct sockaddr_in toAddr;
        uint64_t sessionid;
        uint8_t *data;
        uint32_t len;

        SendPack() {
            memset(&toAddr, 0, sizeof(toAddr));
            sessionid = 0;
            data = NULL;
            len = 0;
        }
    } SendPack;

    int m_fd;
    int m_fdCloseRequested;
    int m_timeoutms ;

    int m_pipefd[2];
    pthread_mutex_t m_mutexSendQueue;
    rqueue<SendPack *, 10> sendQueue;

    std::map<int64_t, Sender *> m_mapSender;
    std::map<int64_t, Receiver *> m_mapReceiver;
    std::list<TimeoutEvent *> m_eventTimerList;

    int deleteTimeoutEvent(TimeoutEvent *timeoutEvent) {
        for (std::list<TimeoutEvent *>::iterator it = m_eventTimerList.begin(); it != m_eventTimerList.end(); it++) {
            if (*it == timeoutEvent) {
                m_eventTimerList.erase(it);
                return 0;
            }
        }
        perr("event delete failed arg:%s", timeoutEvent->toString().c_str());
        return -1;
    }

    int addTimeoutEvent(TimeoutEvent *timeoutEvent) {
        std::list<TimeoutEvent *>::iterator it;
        for (it = m_eventTimerList.begin(); it != m_eventTimerList.end(); it++) {
            if (timeoutEvent->timeout < (*it)->timeout) {
                break;
            }
        }
        m_eventTimerList.insert(it, timeoutEvent);
        return 0;
    }

    int processTimeout(TimeoutEvent *timeoutEvent) {
        const int64_t iSender = sockaddrToint64(timeoutEvent->toaddr);
        Sender *const ss = m_mapSender.count(iSender) ? m_mapSender[iSender] : NULL;

        const std::string strAddr = sockaddrToString(timeoutEvent->toaddr);
        if (!ss) {
            perr("Invalid socket. Not in the list.strAddr:%s ", strAddr.c_str());
            return -1;
        }

        if (timeoutEvent->udpPkg->type == PACKAGE_SYN && timeoutEvent != &ss->synTimeoutEvent) {
            perr("timeoutEvent:%s != &ss->synTimeoutEvent:%s", timeoutEvent->toString().c_str(), ss->synTimeoutEvent.toString().c_str());
            return -1;
        }
        if (timeoutEvent->udpPkg->type == PACKAGE_FIN && timeoutEvent != &ss->finTimeoutEvent) {
            perr("timeoutEvent:%s != &ss->finTimeoutEvent:%s", timeoutEvent->toString().c_str(), ss->finTimeoutEvent.toString().c_str());
            return -1;
        }
        if (timeoutEvent->udpPkg->type == PACKAGE_DATA) {
            TimeoutEvent *tv = getWindowBySeq(ss->dataWindow, timeoutEvent->udpPkg->seq);
            if (timeoutEvent != tv) {
                perr("TIME_Win_not_found %s %s win:%s", timeoutEvent->toString().c_str(), ss->toString().c_str(),
                     getWindowSeqList(ss->dataWindow).c_str());
                return -1;
            }
        }

        pwrn("TIMEOUT: %s  %s", timeoutEvent->toString().c_str(), timeoutEvent->tryCount >= CONST_MAXRETRANS ? "!MAXRETRANS!" : "");
        if (timeoutEvent->tryCount >= CONST_MAXRETRANS) {
            recvEvent(EVENT_TIMEOUT, &timeoutEvent->toaddr, timeoutEvent->udpPkg->sessionid, NULL, 0);
            return 0;
        }
        timeoutEvent->tryCount++;
        return sendPackage(&timeoutEvent->toaddr, timeoutEvent->udpPkg->type, timeoutEvent->udpPkg->seq, timeoutEvent->udpPkg);
    }

    int cleanReceiver(Receiver *rs) {
        if (!rs) {
            return 0;
        }
        m_mapReceiver.erase(sockaddrToint64(rs->fromaddr));
        for (std::map<int64_t, std::pair<uint32_t, uint8_t *> *>::iterator it = rs->sessionMap.begin(); it != rs->sessionMap.end(); it++) {
            std::pair<uint32_t, uint8_t *> *p = (*it).second;
            delete[] p->second;
            delete p;
        }
        delete rs;
        return 1;
    }

    // #lizard forgives
    int processReceive() {
        struct sockaddr_in sender;
        socklen_t sender_length = sizeof(struct sockaddr_in);
        UdpPkg udpPkg;
        const int recvRet = (int) recvfrom(m_fd, &udpPkg, sizeof(UdpPkg), 0, (struct sockaddr *) &sender, &sender_length);

        const std::string strAddr = sockaddrToString(sender);
        pdbg("RECV ret:%d from:%s fd:%d %s", recvRet, strAddr.c_str(), m_fd, udpPkg.toString().c_str());

        const uint32_t crc = udpPkg.datacrc;
        udpPkg.datacrc = 0;
        if (recvRet <= 0 || getCrc32((uint8_t *) &udpPkg, udpPkg.pkgLen()) != crc) {
            perr("IGNORERecv FAULTPkg ret:%d datalen:%d datacrc:0x%x", recvRet, udpPkg.datalen, crc);
            return 0;
        }

        const int64_t iSender = sockaddrToint64(sender);
        Receiver *rs = m_mapReceiver.count(iSender) ? m_mapReceiver[iSender] : NULL;

        if (udpPkg.type == PACKAGE_SYN) {
            if (rs && rs->status != OPENING) { //may be new
                pwrn("R_CLEANNewSync %s RS:%s", udpPkg.toString().c_str(), rs ? rs->toString().c_str() : "NUL_RS");
                cleanReceiver(rs);
                rs = NULL;
            }
            if (!rs) {
                rs = new Receiver();
                rs->fromaddr = sender;
                m_mapReceiver[sockaddrToint64(sender)] = rs;
                pwrn("R_NEW RECEIVER:%s", rs->toString().c_str());
            }
            rs->status = OPENING;
            rs->expectedSeq = udpPkg.seq + 1;
            udpPkg.datalen = 0;
            return sendPackage(&sender, PACKAGE_ACK, udpPkg.seq, &udpPkg);
        }

        if (udpPkg.type == PACKAGE_DATA) {

            if ((!rs) || (rs->status != OPENING && rs->status != OPEN)) {
                pwrn("R_IGNORERecvStat %s RS:%s", udpPkg.toString().c_str(), rs ? rs->toString().c_str() : "NUL_RS");
                return 0;
            }
            if ((udpPkg.seq < (rs->expectedSeq - CONST_WINDOW)) || (udpPkg.seq > rs->expectedSeq)) { //check in window
                pwrn("R_IGNORERecvSEQ [%d,%d] %s RS:%s", udpPkg.seq, rs->expectedSeq, udpPkg.toString().c_str(), rs->toString().c_str());
                return 0;
            }

            if (udpPkg.seq == rs->expectedSeq) {
                if (rs->status == OPENING) rs->status = OPEN;
                rs->expectedSeq = udpPkg.seq + 1;
                dataRecv(&udpPkg, rs);
            }
            udpPkg.offset += udpPkg.datalen; // tell sender , i recv this pkg
            udpPkg.datalen = 0;
            return sendPackage(&sender, PACKAGE_ACK, udpPkg.seq, &udpPkg);
        }

        if (udpPkg.type == PACKAGE_FIN) {
            if (!rs) {
                pwrn("R_IGNORERecvRSNull: %s maybe retrans fin pkg, ack it anyway.", udpPkg.toString().c_str());
                return sendPackage(&sender, PACKAGE_ACK, udpPkg.seq, &udpPkg);
            }
            if (udpPkg.seq != rs->expectedSeq) {
                pwrn("R_IGNORERecvSEQ [%d,%d] %s RS:%s", udpPkg.seq, rs->expectedSeq, udpPkg.toString().c_str(), rs->toString().c_str());
                return 0;
            }

            udpPkg.datalen = 0;
            int ret = sendPackage(&sender, PACKAGE_ACK, udpPkg.seq, &udpPkg);

            pwrn("R_CLOSE RECEIVER:%s", rs->toString().c_str());
            cleanReceiver(rs);
            return ret;
        }

        Sender *const ss = m_mapSender.count(iSender) ? m_mapSender[iSender] : NULL;

        if (udpPkg.type != PACKAGE_ACK || ss == NULL) {
            pwrn("S_IGNORERecvStat %s SS:%s", udpPkg.toString().c_str(), ss ? ss->toString().c_str() : "NUL_SS");
            return 0;
        }

        if (ss->status == FIN_SENT) {
            if (udpPkg.seq != ss->seq) {
                pwrn("S_IGNORERecvSEQ [%d,%d] %s SS:%s", udpPkg.seq, ss->seq, udpPkg.toString().c_str(), ss->toString().c_str());
                return 0;
            }
            pwrn("S_CLOSE SENDER:%s", ss->toString().c_str());

            deleteTimeoutEvent(&ss->finTimeoutEvent);
            delete ss->finTimeoutEvent.udpPkg;
            m_mapSender.erase(iSender);
            return 0;
        }

        if (ss->status == SYN_SENT) {
            if (ss->synTimeoutEvent.udpPkg == NULL || udpPkg.seq != ss->synTimeoutEvent.udpPkg->seq) {
                pwrn("S_IGNORERecvSEQ [%d,%d] %s SS:%s", udpPkg.seq, ss->synTimeoutEvent.udpPkg == NULL ? -1 : ss->synTimeoutEvent.udpPkg->seq,
                        udpPkg.toString().c_str(), ss->toString().c_str());
                return 0;
            }
            deleteTimeoutEvent(&ss->synTimeoutEvent);
            delete ss->synTimeoutEvent.udpPkg;
            ss->status = OPEN;
            return 0;
        }

        if (ss->status == OPEN) {
            TimeoutEvent *timeoutEvent = getWindowBySeq(ss->dataWindow, udpPkg.seq);
            if (!timeoutEvent) {
                pwrn("S_IGNORERecvWIN %s SS:%s win:%s", udpPkg.toString().c_str(), ss->toString().c_str(),
                     getWindowSeqList(ss->dataWindow).c_str());
                return 0;
            }
            deleteTimeoutEvent(timeoutEvent);
            delete timeoutEvent->udpPkg;
            timeoutEvent->udpPkg = NULL;

            if (udpPkg.totallen == udpPkg.offset) {
                recvEvent(EVENT_SESSION_FINISH, &sender, udpPkg.sessionid, NULL, 0);
            }
            return 0;
        }
        perr("NEVER shuold be here :%s", udpPkg.toString().c_str());
        return 0;
    }

    std::string getWindowSeqList(TimeoutEvent *window) {
        std::stringstream s;
        for (int i = 0; i < CONST_WINDOW; i++) {
            s << (window[i].udpPkg ? window[i].udpPkg->seq : -1);
            s << ";";
        }
        return s.str();
    }

    TimeoutEvent* getWindowBySeq(TimeoutEvent *window, uint32_t seq) {
        for (uint32_t i = 0; i < CONST_WINDOW; i++) {
            if (window[i].udpPkg && window[i].udpPkg->seq == seq) {
                return &window[i];
            }
        }
        return NULL;
    }

    TimeoutEvent* checkEmptyWindow(TimeoutEvent *window, int expectSeq) {
        uint32_t minSeq = INT_MAX;
        int emptyIndex = -1;
        for (int i = 0; i < CONST_WINDOW; i++) {
            if (!window[i].udpPkg) {
                emptyIndex = i;
            } else if (minSeq > window[i].udpPkg->seq) {
                minSeq = window[i].udpPkg->seq;
            }
        }

        if (emptyIndex == -1) {
            return NULL;
        }
        if ((minSeq != INT_MAX) && (static_cast<int>(minSeq + CONST_WINDOW - 1) < expectSeq)) {
            return NULL;
        }
        return &window[emptyIndex];
    }

    int windowAllEmpty(TimeoutEvent *window) {
        for (uint32_t i = 0; i < CONST_WINDOW; i++) {
            if (window[i].udpPkg) {
                return 0;
            }
        }
        return 1;
    }

    int sendPackage(struct sockaddr_in *addr, uint8_t type, u_int32_t seq, UdpPkg *udpPkg) {
        if ((!udpPkg) || (!addr)) {
            perr("sendpackage pkg:%d || addr:%d", (uint32_t) ((uint64_t)udpPkg & 0xffffffffL) , (int) ((uint64_t)addr & 0xffffffffL) );
            return -1;
        }

        udpPkg->version = CONST_VERSION;
        udpPkg->type = type;
        udpPkg->seq = seq;
        udpPkg->datacrc = 0;
        udpPkg->datacrc = getCrc32((uint8_t *) udpPkg, udpPkg->pkgLen());

        pdbg("SEND  type:%s:%d seq:%d datalen:%d datacrc:0x%x ", packageTypeStr(type), type, seq, udpPkg->datalen, udpPkg->datacrc);

        const int sendRet = (int) sendto(m_fd, udpPkg, udpPkg->pkgLen(), 0, (struct sockaddr *) addr, sizeof(struct sockaddr));

        const std::string strAddr = sockaddrToString(*addr);
        pdbg("SEND type:%s seq:%d addr:%s fd:%d len:%d sendRet:%d %s", packageTypeStr(udpPkg->type), udpPkg->seq,
             strAddr.c_str(), m_fd, udpPkg->pkgLen(), sendRet,  "");

        if (udpPkg->type == PACKAGE_ACK) {
            return sendRet; // Set a timeout event if the packet isn't an ACK
        }

        const int64_t iSender = sockaddrToint64(*addr);
        Sender *const ss = m_mapSender.count(iSender) ? m_mapSender[iSender] : NULL;
        if (!ss) {
            perr("Invalid socket. Not in the list. addr:%s fd:%d", strAddr.c_str(), m_fd);
            return -1;
        }
        TimeoutEvent *pEvent = NULL;
        if (udpPkg->type == PACKAGE_SYN) {
            pEvent = &ss->synTimeoutEvent;
        } else if (udpPkg->type == PACKAGE_FIN) {
            pEvent = &ss->finTimeoutEvent;
        } else if (udpPkg->type == PACKAGE_DATA) {
            pEvent = getWindowBySeq(ss->dataWindow, udpPkg->seq);
            if (!pEvent) {
                perr("DataPkgNotFountInWin :%s win:%s", udpPkg->toString().c_str(), getWindowSeqList(ss->dataWindow).c_str());
                return -2;
            }
        } else {
            perr("Invalid sock:%d type:%d", m_fd, udpPkg->type);
            return -3;
        }

        memcpy(&(pEvent->toaddr), addr, sizeof(struct sockaddr_in));
        pEvent->timeout = getMillisecond() + m_timeoutms;
        addTimeoutEvent(pEvent);
        return 0;
    }

    Sender* getSender(sockaddr_in addr) {
        const int64_t iSender = sockaddrToint64(addr);
        Sender *ss = m_mapSender.count(iSender) ? m_mapSender[iSender] : NULL;
        if (ss) {
            return ss;
        }
        ss = new Sender();
        m_mapSender[iSender] = ss;
        ss->status = SYN_SENT;
        ss->toaddr = addr;
        ss->seq = 100000;
        ss->synTimeoutEvent.udpPkg = new UdpPkg();
        sendPackage(&addr, PACKAGE_SYN, ss->seq, ss->synTimeoutEvent.udpPkg);
        pwrn("NEW SENDER  %s", ss->toString().c_str());
        return ss;
    }


    int dataRecv(UdpPkg *udpPkg, Receiver *rs) {
        std::pair<uint32_t, uint8_t *> *sessionPair = rs->sessionMap.count(udpPkg->sessionid) ? rs->sessionMap[udpPkg->sessionid] : NULL;

        if (sessionPair == NULL && udpPkg->offset != 0) {
            perr("Not session:%d sessionPair:%d first package offset:%u", TOINT(udpPkg->sessionid), (int) ((uint64_t)sessionPair & 0xffffffffL) , udpPkg->offset);
            return 0;
        }
        if (sessionPair && udpPkg->offset != sessionPair->first) {
            perr("session:%d offset error:%u local:%u", TOINT(udpPkg->sessionid), udpPkg->offset, sessionPair->first);
            return 0;
        }

        if (!sessionPair) {
            sessionPair = new std::pair<uint32_t, uint8_t *>(0, new uint8_t[udpPkg->totallen]);
            rs->sessionMap[udpPkg->sessionid] = sessionPair;
        }
        memcpy(sessionPair->second + sessionPair->first, udpPkg->data, udpPkg->datalen);
        sessionPair->first = udpPkg->offset + udpPkg->datalen;

        if (sessionPair->first == udpPkg->totallen) {
            recvEvent(EVENT_DATA, &rs->fromaddr, udpPkg->sessionid, sessionPair->second, sessionPair->first);
            delete[] sessionPair->second;
            delete sessionPair;
            rs->sessionMap.erase(udpPkg->sessionid);
        }
        return 0;
    }


    int checkSendQueue() {
        SendPack *sendPack = sendQueue.front();

        if (sendPack) {
            if (getSender(sendPack->toAddr)->dataQueue.empty()) {
                sendQueue.pop();
            } else {
                sendPack = NULL;
            }
        }
        int ret = 0;
        if (sendPack) {
            ret = 1;
            uint32_t offset = 0;
            while (offset < sendPack->len) {
                UdpPkg *udpPkg = new UdpPkg();
                udpPkg->sessionid = sendPack->sessionid;
                udpPkg->totallen = sendPack->len;
                udpPkg->offset = offset;
                udpPkg->datalen = (sendPack->len - offset < CONST_MAXPKTSIZE) ? sendPack->len - offset : CONST_MAXPKTSIZE;
                memcpy(udpPkg->data, sendPack->data + offset, udpPkg->datalen);
                offset += udpPkg->datalen;
                getSender(sendPack->toAddr)->dataQueue.push(udpPkg);
            }
            delete[] sendPack->data;
            delete sendPack;
        }

        for (std::map<int64_t, Sender *>::iterator it = m_mapSender.begin(); it != m_mapSender.end(); it++) {
            Sender *ss = it->second;

            while (ss->dataQueue.size()) {
                TimeoutEvent *timeoutEvent = checkEmptyWindow(ss->dataWindow, ss->seq + 1);
                if (timeoutEvent == NULL) {
                    break;
                }

                ss->seq += 1;
                timeoutEvent->tryCount = 0;
                timeoutEvent->udpPkg = ss->dataQueue.front();
                ss->dataQueue.pop();
                sendPackage(&ss->toaddr, PACKAGE_DATA, ss->seq, timeoutEvent->udpPkg);
            }
        }
        return ret;
    }

    int loop() {
        while (m_fd > 0 || m_eventTimerList.size()) {
            const int64_t start = getMillisecond();

            const int checkCount = checkSendQueue();
            const int64_t timeout = m_eventTimerList.size() ? (*m_eventTimerList.begin())->timeout - start : 0;

            pdbg("startLoop ,timeout:%d timer:%d sender:%d checkcnt:[sq:%d %d run:%d] ",
                 TOINT(timeout), (int)m_eventTimerList.size(), (int)m_mapSender.size(), (int)sendQueue.size(), checkCount, TOINT(getMillisecond() - start));

            if (m_eventTimerList.size() && timeout <= 0) { //timeout
                const std::list<TimeoutEvent *>::iterator it = m_eventTimerList.begin();
                int ret = processTimeout(*it);
                pdbg("processTimeout: timeout:%d ret:%d run:%d %s", TOINT(timeout), ret, TOINT(getMillisecond() - start), (*it)->toString().c_str());
                m_eventTimerList.erase(it);
                if (ret < 0) {
                    return -1;
                }
                continue;
            }

            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(m_fd, &fdset);
            FD_SET(m_pipefd[0], &fdset);

            struct timeval diff, *pdiff = NULL;
            if (m_eventTimerList.size() && timeout > 0) {
                diff.tv_sec = timeout / 1000;
                diff.tv_usec = (timeout % 1000) * 1000;
                pdiff = &diff;
            }
            const int selectRet = select(std::max(m_fd, m_pipefd[0]) + 1, &fdset, NULL, NULL, pdiff);
            pdbg("SelectRet:%d err:%d time:%d realTime:%d diff[%d %d]", selectRet, errno, TOINT(timeout), TOINT(getMillisecond() - start),
                 TOINT(diff.tv_sec), TOINT(diff.tv_usec));

            if (selectRet == -1 && errno != EINTR) {
                pdbg("eventloop: select errno:%d", errno);
                continue;
            }

            if (FD_ISSET(m_fd, &fdset)) {
                int ret = processReceive();
                pdbg("eventloop: processReceive ret:%d  run:%d", ret, TOINT(getMillisecond() - start));
                if (ret < 0) {
                    return -1;
                }
            }

            if (FD_ISSET(m_pipefd[0], &fdset)) {
                int i = 0;
                int ret = read(m_pipefd[0], &i, sizeof(int));
                pdbg("read pipe ret:%d run:%d ", ret, TOINT(getMillisecond() - start));
            }
        }
        pdbg("end loop , fd:%d timer:%d", m_fd, (int)m_eventTimerList.size());
        return 0;
    }

public:
    virtual ~UDP(){}

    UDP() {
        m_fd = 0;
        m_fdCloseRequested = 0;
        m_timeoutms = 0;
        m_pipefd[0] = 0;
        m_pipefd[1] = 0;
        m_mutexSendQueue = PTHREAD_MUTEX_INITIALIZER;
    }

    int createSocket(const char *ip, int port, int timeoutms) {
        if (m_fd > 0) {
            return m_fd;
        }

        this->m_timeoutms = timeoutms;
        srand(time(NULL));

        pthread_mutex_init(&m_mutexSendQueue, NULL);
        int ret = pipe(m_pipefd);
        if (ret < 0) {
            perr("create pipe ret:%d", ret);
            return -1;
        }

        const int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0) {
            perr("create socket fd:%d", fd);
            return -1;
        }

        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        if (ip == NULL || strlen(ip) == 0) {
            address.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else {
            inet_pton(AF_INET, ip, &(address.sin_addr));
        }
        const int bindRet = ::bind(fd, (struct sockaddr *) &address, (socklen_t)sizeof(address));
        if (bindRet < 0) {
            perr("create socket bindRet:%d fd:%d %s:%d ", bindRet, fd, ip, port);
            close(fd);
            return -2;
        }

        m_fd = fd;
        m_fdCloseRequested = false;
        pwrn("Create socket succ. fd:%d %s:%d pipe:[%d,%d]", fd, ip, port, m_pipefd[0], m_pipefd[1]);

        return fd;
    }

    int startLoop() {
        return loop();
    }

    int64_t send(struct sockaddr_in addr, uint8_t *data, int len) {
        SendPack *sendPack = new SendPack();

        struct timeval now;
        gettimeofday(&now, NULL);
        sendPack->sessionid = (((int64_t) now.tv_sec) * 1000 * 1000) + now.tv_usec;

        sendPack->toAddr = addr;
        sendPack->len = len;
        sendPack->data = new uint8_t[len];
        memcpy(sendPack->data, data, len);

        sendQueue.push(sendPack);
        write(m_pipefd[1], &m_pipefd[1], sizeof(int));

        return sendPack->sessionid;
    }


};

#endif //AMC_UDP_H
