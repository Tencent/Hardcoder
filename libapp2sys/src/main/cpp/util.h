/**
 * Utility for common operation(debug, log, numeric conversion)
 */
#ifndef HARDCODER_UTIL_H_H
#define HARDCODER_UTIL_H_H

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

char const *TAG = "HARDCODER"; // default log tag
static void setTag(char const *tag) {
    TAG = tag;
}

static bool DEBUG = true; // global debug flag for native code default(false)

#define UNUSED(x) (void)(x)
#define TOINT(x) ((int)( ((int64_t)x) & 0xffffffffL )  )
#define MIN(a,b) (((a)<(b))?(a):(b))

#if defined(ANDROID_NDK)

#include <android/log.h>
#include <fstream>

int (*logFunc)(int prio, const char *tag,  const char *fmt, ...) = &__android_log_print;


#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):  \
                            (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__ ) )

#define perr(format, ...) (*logFunc)(ANDROID_LOG_ERROR , TAG, "[%s,%s:%d]"#format, __FILENAME__,  __FUNCTION__, __LINE__ , ##__VA_ARGS__);
#define pwrn(format, ...) (*logFunc)(ANDROID_LOG_WARN , TAG, "[%s,%s:%d]"#format, __FILENAME__,   __FUNCTION__, __LINE__ , ##__VA_ARGS__);
#define pdbg(format, ...) if (DEBUG)((*logFunc)(ANDROID_LOG_DEBUG , TAG, "[%s,%s:%d]"#format, __FILENAME__ ,  __FUNCTION__, __LINE__ , ##__VA_ARGS__));

#else

#define perr(format, ...) do{struct timeval now; gettimeofday(&now, NULL);tm tm = *localtime((const time_t*)&(now.tv_sec));\
							fprintf(stderr ,"ERR:[%02d:%02d:%02d.%.3ld][%s:%d]"#format"\n", tm.tm_hour, tm.tm_min, tm.tm_sec, now.tv_usec / 1000, \
							  __FUNCTION__, __LINE__ , ##__VA_ARGS__);}while(0);

#define pwrn(format, ...) do{struct timeval now; gettimeofday(&now, NULL);tm tm = *localtime((const time_t*)&(now.tv_sec));\
							fprintf(stderr ,"WRN:[%02d:%02d:%02d.%.3ld][%s:%d]"#format"\n", tm.tm_hour, tm.tm_min, tm.tm_sec, now.tv_usec / 1000, \
							  __FUNCTION__, __LINE__ , ##__VA_ARGS__);}while(0);

#define pdbg(format, ...) if (DEBUG) do{struct timeval now; gettimeofday(&now, NULL);tm tm = *localtime((const time_t*)&(now.tv_sec));\
							fprintf(stderr ,"DBG:[%02d:%02d:%02d.%.3ld][%s:%d]"#format"\n", tm.tm_hour, tm.tm_min, tm.tm_sec, now.tv_usec / 1000, \
							 __FUNCTION__, __LINE__ , ##__VA_ARGS__);}while(0);
#endif


template<typename T, int queuelen>
class rqueue {
private:
    std::deque<T> _q;
    pthread_mutex_t _mutex;
    sem_t _semcnt;
    sem_t _semmax;

public:
    rqueue() {
        sem_init(&_semmax, 0, queuelen ? queuelen : 65535);
        sem_init(&_semcnt, 0, 0);
        pthread_mutex_init(&_mutex, NULL);
    }

    ~rqueue() {
        sem_destroy(&_semmax);
        sem_destroy(&_semcnt);
        pthread_mutex_destroy(&_mutex);
    }

    void push(T obj) {
        sem_wait(&_semmax);
        pthread_mutex_lock(&_mutex);
        _q.push_back(obj);
        pthread_mutex_unlock(&_mutex);
        sem_post(&_semcnt);
    }

    T pop() {
        sem_wait(&_semcnt);
        pthread_mutex_lock(&_mutex);
        T obj = _q.front();
        _q.pop_front();
        pthread_mutex_unlock(&_mutex);
        sem_post(&_semmax);
        return obj;
    }

    T front() {
        pthread_mutex_lock(&_mutex);
        T obj = _q.size() ? _q.front() : NULL;
        pthread_mutex_unlock(&_mutex);
        return obj;
    }

    unsigned int size() {
        pthread_mutex_lock(&_mutex);
        unsigned int s = _q.size();
        pthread_mutex_unlock(&_mutex);
        return s;
    }
};

static int64_t getMillisecond() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (((int64_t) now.tv_sec) * 1000) + now.tv_usec / 1000;
}

static std::string sockaddrToString(struct sockaddr_in addr) {
    char a[32] = {0};
    unsigned char *bytes = (unsigned char *) &addr.sin_addr;
    snprintf(a, sizeof(a), "%d.%d.%d.%d:%d", bytes[0], bytes[1], bytes[2], bytes[3], ntohs(addr.sin_port));
    return (std::string) a;
}

static sockaddr_in toSockaddr(const char *ip, uint16_t port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &(addr.sin_addr));
    return addr;
}

static int sockaddrToIpPort(struct sockaddr_in addr, std::string *ip, int *pPort) {
    char a[32] = {0};
    unsigned char *bytes = (unsigned char *) &addr.sin_addr;
    snprintf(a, sizeof(a), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    *ip = a;
    *pPort = ntohs(addr.sin_port);
    return 0;
}

static int64_t sockaddrToint64(struct sockaddr_in addr) {
    int64_t s = addr.sin_port;
    return (s << 32) + addr.sin_addr.s_addr;
}

static const uint8_t crc4_table[16] = {0, 3, 6, 5, 12, 15, 10, 9, 11, 8, 13, 14, 7, 4, 1, 2};

static uint32_t getcrc4(void *data, uint32_t len) {  // RETURN  4 bit crc
    uint8_t *p = (uint8_t *) data;

    uint8_t crc = 0;
    for (uint32_t k = 0; k < len; k++) {
        crc ^= p[k] >> 4;
        crc = crc4_table[crc];
        crc ^= p[k] & 0xF;
        crc = crc4_table[crc];
    }
    return crc;
}

static unsigned int Crc32Table[256] = {0};

static unsigned int getCrc32(uint8_t *data, uint32_t len) {
    int i, j;
    unsigned int Crc;
    if (Crc32Table[0] == 0) {    //create CRC32 table
        for (i = 0; i < 256; i++) {
            Crc = i;
            for (j = 0; j < 8; j++) {
                if (Crc & 1)
                    Crc = (Crc >> 1) ^ 0xEDB88320;
                else
                    Crc >>= 1;
            }
            Crc32Table[i] = Crc;
        }
    }

    Crc = 0xffffffff;
    for (uint32_t i = 0; i < len; i++) {
        Crc = (Crc >> 8) ^ Crc32Table[(Crc & 0xFF) ^ (data[i])];
    }

    Crc ^= 0xFFFFFFFF;
    return Crc;
}

static char* getVersion(const char* str, const char* delims) {
    if (str == NULL || delims == NULL) {
        return NULL;
    }
    size_t str_len = strlen(str);
    char* result = new char[str_len + 1];
    if (result == NULL) {
        return NULL;
    }
    const char* delim_ptr = strstr(str, delims);
    if (delim_ptr != NULL) {
        size_t delims_len = strlen(delims);
        size_t prefix_len = ((unsigned long) delim_ptr - (unsigned long) str) / sizeof(char) + 1;
        size_t suffix_len = str_len - prefix_len - delims_len + 1;
        strncpy(result, delim_ptr + delims_len, suffix_len);
        result[suffix_len] = '\0';
    } else {
        strncpy(result, str, str_len);
        result[str_len] = '\0';
    }
    return result;
}

static char* getSocketName(const char* str, const char* delims) {
    if (str == NULL || delims == NULL) {
        return NULL;
    }
    size_t str_len = strlen(str);
    char* result = new char[str_len + 1];
    if (result == NULL) {
        return NULL;
    }
    const char* delim_ptr = strstr(str, delims);
    if (delim_ptr != NULL) {
        size_t prefix_len = ((unsigned long) delim_ptr - (unsigned long) str) / sizeof(char);
        strncpy(result, str, prefix_len);
        result[prefix_len] = '\0';
    } else {
        strncpy(result, str, str_len);
        result[str_len] = '\0';
    }
    return result;
}

class LocalPortCheck {

public:

    const static int TCP_PORT = 1;
    const static int UDP_PORT = 2;

    static int getUidByPort(const int type, const int port) {

        char szPort[8];
        snprintf(szPort, sizeof(szPort), "%X", port);
        perr("port type:%d port:%x hex:%s", type, port, szPort);

        char const *path = NULL;
        if (type == TCP_PORT) path = "/proc/net/tcp";
        else if (type == UDP_PORT) path = "/proc/net/udp";
        else return -2;


        FILE *fp = fopen(path, "r");
        if (!fp) {
            return -3;
        }

        char line[4096];
        while (fgets(line, sizeof(line), fp)) {

            char *saveptr = line;
            char *token = NULL;
            int portHitRow = 0;
            int colIndex = 0;
            while ((token = strtok_r(saveptr, " ", &saveptr)) != NULL) {

                if (colIndex == 1) {
                    char *saveptr2 = token;
                    char *token2 = NULL;
                    while ((token2 = strtok_r(saveptr2, ":", &saveptr2)) != NULL) {
                        if (!strcmp(szPort, token2)) portHitRow = 1;

                    }
                }
                if (portHitRow == 1 && colIndex == 7) {
                    fclose(fp);
                    return atoi(token);
                }
                colIndex++;
            }
        }
        fclose(fp);
        return -1;
    }
};

#endif //HARDCODER_UTIL_H_H
