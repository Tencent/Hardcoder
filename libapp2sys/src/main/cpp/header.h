/**
 * req and resp packer header
 */

#ifndef HARDCODER_HEADER_H
#define HARDCODER_HEADER_H

#include "util.h"

static const uint16_t HEADER_PROTOCAL_VERSION = 4;
static const uint16_t HEADER_JSON_VERSION = 16;


static const uint16_t HEADER_BODYFORMAT_RAW = 1;
static const uint16_t HEADER_BODYFORMAT_PROTO = 2;
static const uint16_t HEADER_BODYFORMAT_JSON = 3;


static const uint32_t HEADER_BEGIN = 0x48444352; //HDCR

uint16_t HEADER_VERSION = HEADER_PROTOCAL_VERSION;

typedef struct AMCReqHeader {
    uint32_t begin;     //包头其起始字段
    uint16_t version;   //协议版本
    uint16_t funcid;    //请求对应的function ID
    uint32_t bodylen;   //包体序列化数据长度
    int64_t requestid;  //当前请求包ID
    uint32_t callertid; //上层JNI调用者所在线程ID
    int64_t timestamp;  //当前请求时间戳

}__attribute__ ((packed)) AMCReqHeader;


typedef struct AMCReqHeaderV2: AMCReqHeader{
    uint32_t headerlen;     //包头数据长度（Ver2新增）
	uint32_t bodyformat;    //包体数据序列化格式枚举值（Ver2新增）
	uint32_t data1;         //预留字段1
	uint32_t data2;         //预留字段2
	uint32_t data3;         //预留字段3

}__attribute__ ((packed)) AMCReqHeaderV2;


static int64_t genReqPack(uint32_t funcid, uint8_t *data, int dataLen, uint8_t **outPack, uint32_t *outLen, uint32_t callertid, int64_t timestamp) {
    pdbg("getReqPack funcid:%d, dataLen:%d callertid:%d timestamp:%d", funcid, dataLen, callertid, TOINT(timestamp/1000000L));

    if (dataLen > 0 && data == NULL) {
        perr("getReqPack input data len %d data:%d", dataLen,  TOINT(data));
        return -1;
    }

    if (funcid > 65535) {
        perr("getReqPack funcid should less than 65535, funid:%d", funcid);
        return -2;
    }

    if (HEADER_VERSION < HEADER_PROTOCAL_VERSION) {
        perr("getReqPack HEADER_VERSION err:%d", HEADER_VERSION);
        return -1;
    }

    if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
        uint32_t totalLen = dataLen + sizeof(AMCReqHeaderV2);
        uint8_t *buf = new uint8_t[totalLen];

        AMCReqHeaderV2 *header = reinterpret_cast<AMCReqHeaderV2 *>(buf);
        header->begin = HEADER_BEGIN;
        header->version = HEADER_VERSION;
        header->funcid = funcid;
        header->bodylen = dataLen;
        header->callertid = callertid;
        header->timestamp = timestamp;

        header->headerlen = sizeof(AMCReqHeaderV2);
        header->bodyformat = HEADER_BODYFORMAT_JSON;
        struct timeval now;
        gettimeofday(&now, NULL);
        header->requestid = (static_cast<int64_t>(now.tv_sec) * 1000 * 1000) + now.tv_usec;
        if (dataLen) {
            memcpy(buf + sizeof(AMCReqHeaderV2), data, dataLen);
        }

        *outPack = buf;
        *outLen = totalLen;

        pdbg("genReqPack json header len:%d, requestid:%lld", header->headerlen, header->requestid);

        return header->requestid;
    } else {//proto
        uint32_t totalLen = dataLen + sizeof(AMCReqHeader);
        uint8_t *buf = new uint8_t[totalLen];

        AMCReqHeader *header = reinterpret_cast<AMCReqHeader *>(buf);
        header->begin = HEADER_BEGIN;
        header->version = HEADER_VERSION;
        header->funcid = funcid;
        header->bodylen = dataLen;
        header->callertid = callertid;
        header->timestamp = timestamp;
        struct timeval now;
        gettimeofday(&now, NULL);
        header->requestid = (static_cast<int64_t>(now.tv_sec) * 1000 * 1000) + now.tv_usec;
        if (dataLen) {
            memcpy(buf + sizeof(AMCReqHeader), data, dataLen);
        }

        *outPack = buf;
        *outLen = totalLen;

        pdbg("genReqPack pb header len:%zu requestid:%lld", sizeof(AMCReqHeader), header->requestid);
        return header->requestid;
    }

}


typedef struct AMCRespHeader {
    uint32_t begin;     //包头其起始字段
    uint16_t version;   //协议版本
    uint16_t funcid;    //响应请求对应的function ID
    uint32_t retCode;   //请求处理结果
    uint32_t bodylen;   //包体序列化数据长度
    int64_t requestid;  //响应对应的请求包ID
    int64_t timestamp;  //当前响应时间戳

}__attribute__ ((packed)) AMCRespHeader;


typedef struct AMCRespHeaderV2: AMCRespHeader{
    uint32_t headerlen;     //包头数据长度（Ver2新增）
	uint32_t bodyformat;    //包体数据序列化格式枚举值（Ver2新增）
	uint32_t data1;         //预留字段1
	uint32_t data2;         //预留字段2
	uint32_t data3;         //预留字段3

}__attribute__ ((packed)) AMCRespHeaderV2;


static int64_t genRespPack(uint32_t funcid, uint32_t retCode, uint64_t requestId, uint8_t *data, int dataLen,
                           uint8_t **outPack, uint32_t *outLen) {

    if (funcid > 65535) {
        perr("genRespPack funcid should less than 65535, funid:%d", funcid);
        return -2;
    }

    if (HEADER_VERSION < HEADER_PROTOCAL_VERSION) {
        perr("genRespPack HEADER_VERSION err:%d", HEADER_VERSION);
        return -1;
    }

    if (HEADER_VERSION >= HEADER_JSON_VERSION) {//json
        uint32_t totalLen = dataLen + sizeof(AMCRespHeaderV2);
        uint8_t *buf = new uint8_t[totalLen];

        AMCRespHeaderV2 *p = reinterpret_cast<AMCRespHeaderV2 *>(buf);
        p->retCode = retCode;
        p->requestid = requestId;
        p->funcid = funcid;
        p->bodylen = dataLen;
        struct timespec ts = {0, 0};
        clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
        p->timestamp = (static_cast<int64_t>(ts.tv_sec) * 1000 * 1000 * 1000) + ts.tv_nsec;
        pdbg("genRespPack json funcid:%d timestamp:%d ms requestId:%lld", funcid, TOINT(p->timestamp/1000000L), requestId);
        p->begin =  HEADER_BEGIN;
        p->version = HEADER_VERSION;

        p->headerlen = sizeof(AMCRespHeaderV2);
        p->bodyformat = HEADER_BODYFORMAT_JSON;

        if (dataLen) {
            memcpy(buf + sizeof(AMCRespHeaderV2), data, dataLen);
        }
        *outPack = buf;
        *outLen = totalLen;

    } else {//proto
        uint32_t totalLen = dataLen + sizeof(AMCRespHeader);
        uint8_t *buf = new uint8_t[totalLen];
        AMCRespHeader *p = reinterpret_cast<AMCRespHeader *>(buf);
        p->retCode = retCode;
        p->requestid = requestId;
        p->funcid = funcid;
        p->bodylen = dataLen;
        struct timespec ts = {0, 0};
        clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
        p->timestamp = (static_cast<int64_t>(ts.tv_sec) * 1000 * 1000 * 1000) + ts.tv_nsec;
        pdbg("genRespPack proto funcid:%d timestamp:%d ms requestId:%lld", funcid, TOINT(p->timestamp/1000000L), requestId);
        p->begin =  HEADER_BEGIN;
        p->version = HEADER_VERSION;

        if (dataLen) {
            memcpy(buf + sizeof(AMCRespHeader), data, dataLen);
        }
        *outPack = buf;
        *outLen = totalLen;

    }
    return requestId;

}

#endif //HARDCODER_HEADER_H
