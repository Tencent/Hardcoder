/**
 * Utility for proc IO settings (cpu frea, stat)
 */

#ifndef HARDCODER_PROCUTIL_H
#define HARDCODER_PROCUTIL_H

#include "util.h"
#include <sys/types.h>
#include <dirent.h>

/*
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies

echo "0" > /sys/devices/system/cpu/cpu0/online

echo 1248000  > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
echo 1248000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq

echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor


cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq;

cat /sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu5/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu6/cpufreq/scaling_cur_freq;
cat /sys/devices/system/cpu/cpu7/cpufreq/scaling_cur_freq;


cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu2/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu3/cpufreq/cpuinfo_max_freq;

cat /sys/devices/system/cpu/cpu4/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu5/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu6/cpufreq/cpuinfo_max_freq;
cat /sys/devices/system/cpu/cpu7/cpufreq/cpuinfo_max_freq;
*/

static int checkCpuCount() {
    for (int i = 0; i < 128; i++) {
        char buf[64];
        memset(buf, 0, 64);
        snprintf(buf, 63, "/sys/devices/system/cpu/cpu%d", i);

        DIR *d = opendir(buf);
        if (d) {
            closedir(d);
        } else {
            return i;
        }
    }
    return 0;
}

const static int CPU_COUNT = checkCpuCount();
const static uint32_t STAT_USER_JIFFIES_INDEX = 13;
const static uint32_t STAT_SYS_JIFFIES_INDEX = 14;

class ProcFileReader {

private:
    char *m_szPath;
    int fd;
    char *line;

    const static uint32_t MAX_LEN = 4096;

public:
    ProcFileReader(const char *path) {
        m_szPath = NULL;
        fd = 0;
        line = NULL;

        if (path) {
            int len = strlen(path);
            if (len) {
                m_szPath = new char[len + 1];
                strcpy(m_szPath, path);
                line = new char[MAX_LEN];
            }
        }
    }

    ~ProcFileReader() {
        if (m_szPath) {
            delete[] m_szPath;
            m_szPath = NULL;
        }
        if (fd) {
            close(fd);
            fd = 0;
        }
        if (line) {
            delete[] line;
            line = NULL;
        }
    }

    const char* getPath() {
        return m_szPath;
    }

    char* update() {
        if (!m_szPath) {
            return NULL;
        }

        if (!fd) {
            fd = open(m_szPath, O_RDONLY);
        } else {
            lseek(fd, 0, SEEK_SET);
        }

        if (fd < 0) {
            fd = 0;
            return NULL;
        }

        int ret = read(fd, line, MAX_LEN);
        if (ret <= 0) {
            close(fd);
            fd = 0;
        }
        line[ret] = '\0';
        return line;
    }

    int64_t getValue(uint32_t index) {
        char *ptr = line;
        uint32_t spaceCount = 0;
        while (*ptr != '\0') {
            if (*ptr == ' ') {
                spaceCount++;
            }
            if (spaceCount == index) {
                return atoi(ptr);
            }
            ptr++;
        }
        return -1;
    }
};


class ThreadJiffiesMonitor {

private:
    uint32_t m_running;
    pthread_t m_thr;

    int m_lastJiffies;
    uint64_t m_startUnixTime;
    uint32_t m_sumCpuLoad;

    ProcFileReader **m_ppCpuFfreqReader;
    ProcFileReader *m_pStatReader;
    uint32_t m_linuxTid;
    uint64_t m_sampleRateUS;


    static void* monitorThread(void *t) {
        ThreadJiffiesMonitor *thiz = (ThreadJiffiesMonitor *) t;
        while (thiz->m_running) {
            thiz->run();
            usleep(thiz->m_sampleRateUS);
        }
        return NULL;
    }

    int run() {
        m_pStatReader->update();
        int32_t j = m_pStatReader->getValue(STAT_USER_JIFFIES_INDEX) + m_pStatReader->getValue(STAT_SYS_JIFFIES_INDEX);
        int cpuId = m_pStatReader->getValue(38);

        if (j < 0 || cpuId < 0 || cpuId >= CPU_COUNT) {
            return -1;
        }

        m_ppCpuFfreqReader[cpuId]->update();
        int64_t freqHz = m_ppCpuFfreqReader[cpuId]->getValue(0);
        m_sumCpuLoad += (freqHz * (j - m_lastJiffies));

//        uint64_t now = getMillisecond();
//        uint32_t diff = lastRun ? now - lastRun : 0 ;
//        lastRun = now;
//        pdbg("run  diff:%d core:%d freq:%lld j:%d last:%d load:%lld ", diff, cpuId,  freqHz , j , m_lastJiffies  , (freqHz * (j - m_lastJiffies)));

        m_lastJiffies = j;
        return 0;
    }

public:
    int start(uint32_t linuxTid, uint64_t sampleRateMS = 15) {
        m_startUnixTime = getMillisecond();
        m_sumCpuLoad = 0;
        m_linuxTid = linuxTid;
        m_sampleRateUS = sampleRateMS * 1000;

        if (CPU_COUNT <= 0 || m_linuxTid <= 0) {
            return -1;
        }

        char buf[64];
        m_ppCpuFfreqReader = new ProcFileReader *[CPU_COUNT];
        for (int i = 0; i < CPU_COUNT; i++) {
            sprintf(buf, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", i);
            m_ppCpuFfreqReader[i] = new ProcFileReader(buf);
        }
        sprintf(buf, "/proc/self/task/%d/stat", m_linuxTid);
        m_pStatReader = new ProcFileReader(buf);
        m_pStatReader->update();
        m_lastJiffies = m_pStatReader->getValue(STAT_USER_JIFFIES_INDEX) + m_pStatReader->getValue(STAT_SYS_JIFFIES_INDEX);

        if (m_lastJiffies < 0) {
            return -2;
        }

        m_running = 1;
        pthread_create(&m_thr, NULL, monitorThread, this);
        return 0;
    }

    int stop(uint64_t &runUnixTime, uint64_t &cpuLoad) {
        runUnixTime = getMillisecond() - m_startUnixTime;

        m_running = 0;
        void *stat;
        pthread_join(m_thr, &stat);

        cpuLoad = m_sumCpuLoad;

        delete m_pStatReader;
        for (int i = 0; i < CPU_COUNT; i++) {
            delete m_ppCpuFfreqReader[i];
        }
        delete[] m_ppCpuFfreqReader;

        return 0;
    }
};

#endif //HARDCODER_PROCUTIL_H
