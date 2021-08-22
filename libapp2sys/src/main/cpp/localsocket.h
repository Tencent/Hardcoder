#ifndef HARDCODER_LOCALSOCKET_H
#define HARDCODER_LOCALSOCKET_H

#include "util.h"
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <asm/ioctls.h>

/**
 * Localsocket implementation
 */
class LocalSocket {

protected:
    typedef enum {
        EVENT_ERROR, EVENT_DATA, EVENT_SESSION_FINISH, EVENT_CONNECT
    } Event;

    virtual int recvEvent(Event event, int fd, int pid, uid_t uid, const char *path, uint8_t *data, int len) = 0;

private:
    typedef struct SendPack {
        int fd;
        uint8_t *data;
        uint32_t len;
        uint32_t offset;

        SendPack() {
            fd = 0;
            data = NULL;
            len = 0;
            offset = 0;
        }
    } SendPack;

    int m_fd;
    int m_pipefd[2];
    std::map<int, std::pair<uid_t, std::string> > mapUid;

    rqueue<SendPack*, 5> m_sendQueue;


    static int setSocketNonBlock(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            perr("setSocketNonBlock fcntl F_GETFL failed, fd:%d, flag:%d", fd, flags);
            return -1;
        }
        int ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        if (ret < 0) {
            perr("setSocketNonBlock fcntl F_SETFL failed, fd:%d, ret:%d ", fd, ret);
            return -2;
        }
        return 0;
    }

    static int checkCanWrite(int fd) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        select(fd + 1, NULL, &fdset, NULL, &timeout);
        pdbg("checkCanWrite FD_ISSET(fd, &fdset):%d, fd:%d", FD_ISSET(fd, &fdset), fd);
        return FD_ISSET(fd, &fdset);
    }


    int socketAccept(int fd) {
        struct sockaddr_un un;
        socklen_t len = sizeof(un);
        int acceptFd = accept(fd, (struct sockaddr *) &un, &len);
        if (acceptFd < 0) {
            perr("accept error, fd:%d, ret:%d", fd, acceptFd);
            return -1;
        }

        if (0 > setSocketNonBlock(acceptFd)) {
            perr("setSocketNonBlock error, acceptFd:%d", acceptFd);
            close(acceptFd);
            return -2;
        }

        len -= offsetof(struct sockaddr_un, sun_path);
        un.sun_path[len] = 0;
        char *path = un.sun_path + 1; // pos 0 is \0
        struct ucred ucred;
        len = sizeof(struct ucred);
        getsockopt(acceptFd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
        pdbg("socketAccept getsockopt pid:%d, uid:%d, gid:%d, len:%d", ucred.pid, ucred.uid, ucred.gid, len);

        uid_t uid = ucred.uid;
        pwrn("socketAccept fd:%d, acceptFd:%d, uid:%d, path:%s", m_fd, acceptFd, uid, path);

        if (0 > recvEvent(EVENT_CONNECT, acceptFd, ucred.pid, uid, path, NULL, 0)) {
            perr("recvEvent NotAccept, fd:%d, pid:%d, uid:%d, path:%s", acceptFd, ucred.pid, uid, un.sun_path);
            close(acceptFd);
            return -4;
        }

        std::pair<uid_t, std::string> p;
        p.first = uid;
        p.second = un.sun_path;
        mapUid[acceptFd] = p;
        return acceptFd;
    }

    void notifyError(int recvFd) {
        if (recvFd == m_fd) { //client
            recvEvent(EVENT_ERROR, recvFd, 0, 0, NULL, NULL, 0);
            return;
        }
        recvEvent(EVENT_ERROR, recvFd, 0, mapUid[recvFd].first, mapUid[recvFd].second.c_str(), NULL, 0);
        close(recvFd);
    }

    // #lizard forgives
    int loop(const int isServer) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(m_fd, &fdset);
        FD_SET(m_pipefd[0], &fdset);
        int maxfd = m_fd;

        pdbg("loop, m_fd:%d, maxfd:%d, isServer:%d", m_fd, maxfd, isServer);

        while (m_fd) {
            const int64_t startTime = getMillisecond();

            SendPack *sendPack = m_sendQueue.front();
            if (sendPack && checkCanWrite(sendPack->fd)) {

                int ret = send(sendPack->fd, sendPack->data + sendPack->offset, sendPack->len - sendPack->offset, 0);

                if (ret < 0) {
                    perr("loop, send failed, fd:%d, m_fd:%d", sendPack->fd, m_fd);
                    if (sendPack->fd == m_fd) { //client
                        notifyError(sendPack->fd);
                        break;
                    } else {//server
                        m_sendQueue.pop();
                        delete[] sendPack->data;
                        FD_CLR(sendPack->fd, &fdset);
                        notifyError(sendPack->fd);
                        delete sendPack;
                        continue;
                    }
                }

                if (ret > 0) {
                    sendPack->offset += ret;
                }

                if (sendPack->offset == sendPack->len) {//this sendPack finish
                    m_sendQueue.pop();
                    pdbg("loop, send sendPack finish, ret:%d, fd:%d, len:%d, offset:%d, queue:%d, costtime:%d ", ret, sendPack->fd, sendPack->len, sendPack->offset,
                         m_sendQueue.size(), TOINT(getMillisecond() - startTime) );
                    delete[] sendPack->data;
                    delete sendPack;
                } else {
                    pdbg("loop, send, ret:%d, fd:%d, len:%d, offset:%d, queue:%d, costtime:%d ", ret, sendPack->fd, sendPack->len, sendPack->offset,
                         m_sendQueue.size(), TOINT(getMillisecond() - startTime) );
                }
            }
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            fd_set tmpSet = fdset;
            int selectRet = select(maxfd + 1, &tmpSet, NULL, NULL, &timeout);
            if (selectRet == -1 && errno != EINTR) {
                pdbg("loop, select errno:%d", errno);
                continue;
            } else if (selectRet == 0) {
                continue;
            } else {
                for (int i = 3; i <= maxfd; i++) {
                    if ((!FD_ISSET(i, &tmpSet)) || (!FD_ISSET(i, &fdset))) {
                        continue;
                    }
                    if (m_pipefd[0] == i) {
                        int l = 0;
                        int ret = read(m_pipefd[0], &l, sizeof(int));
                        UNUSED(ret);
                        pdbg("loop, select success, read pipe ret:%d", ret);
                    } else if (isServer && i == m_fd) {
                        int acceptfd = socketAccept(m_fd);
                        if (acceptfd <= 0) {
                            return -1;
                        }
                        FD_SET(acceptfd, &fdset);
                        maxfd = maxfd > acceptfd ? maxfd : acceptfd;
                        pdbg("loop, select success, socketAccept, acceptfd:%d, maxfd:%d", acceptfd, maxfd);
                    } else {
                        uint8_t szBuf[4096];
                        int ret = recv(i, szBuf, sizeof(szBuf), 0);
                        if (ret <= 0) {
                            perr("loop, recv error, fd:%d, ret:%d, errno:%d", i, ret, errno);
                            FD_CLR(i, &fdset);
                            notifyError(i);
                            continue;
                        }
                        pdbg("loop, select success, recv fd:%d, ret:%d", i, ret);
                        recvEvent(EVENT_DATA, i, 0, mapUid[i].first, mapUid[i].second.c_str(), szBuf, ret);
                    }
                }
            }
        }
        uninit(-1);
        return 0;
    }

protected:
    void uninit(int flag) {
        pwrn("uninit, pipe:[%d,%d], m_fd:%d, queue:%d, flag:%d", m_pipefd[0], m_pipefd[1], m_fd, m_sendQueue.size(), flag);
        if (m_pipefd[0]) {
            close(m_pipefd[0]);
            m_pipefd[0] = 0;
        }
        if (m_pipefd[1]) {
            close(m_pipefd[1]);
            m_pipefd[1] = 0;
        }
        if (m_fd) {
            close(m_fd);
            m_fd = 0;
        }
        while (m_sendQueue.size()) {
            SendPack *s = m_sendQueue.pop();
            delete[] s->data;
            delete s;
        }
    }

public:
    LocalSocket() {
        m_pipefd[0] = 0;
        m_pipefd[1] = 0;
        m_fd = 0;
    }

    virtual ~LocalSocket() {
        uninit(0);
    }

    //server: remotePath is null or ""
    //client: remotePath is valid
    int createSocket(const char *localPath, const char *remotePath) {
        pdbg("createSocket, localPath: %s, remotePath: %s", localPath, remotePath);
        if (m_fd > 0) {
            return m_fd;
        }

        int ret = pipe(m_pipefd);
        if (ret < 0) {
            perr("createSocket, create pipe error, ret:%d", ret);
            return -1;
        }
        const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            perr("createSocket, socket error, fd:%d", fd);
            return -1;
        }

        struct sockaddr_un un;
        socklen_t len;
        if (remotePath == NULL || strlen(remotePath) == 0) { //server
            unlink(localPath);

            memset(&un, 0, sizeof(un));
            un.sun_family = AF_UNIX;
            un.sun_path[0] = '\0';
            memcpy(un.sun_path + 1, localPath, strlen(localPath));
            len = offsetof(struct sockaddr_un, sun_path) + strlen(localPath) + 1;
            ret = ::bind(fd, (struct sockaddr *) &un, len);
            if (ret < 0) {
                close(fd);
                perr("createSocket, bind error, ret:%d, fd:%d, localPath:%s", ret, fd, localPath);
                return -2;
            }

            ret = listen(fd, 5);
            if (ret < 0) {
                close(fd);
                perr("createSocket, listen error, ret:%d, fd:%d, localPath:%s", ret, fd, localPath);
                return -3;
            }
            m_fd = fd;
            pwrn("createSocket, create Server socket:%d, localPath:%s", m_fd, localPath);
            return fd;
        }

        memset(&un, 0, sizeof(un));
        un.sun_family = AF_UNIX;
        un.sun_path[0] = '\0';
        memcpy(un.sun_path + 1, remotePath, strlen(remotePath));
        len = offsetof(struct sockaddr_un, sun_path) + strlen(remotePath) + 1;



//      non-blocking connect implementation
        int opt = 1;
        //set non-blocking
        if (ioctl(fd, FIONBIO, &opt) < 0) {
            close(fd);
            perr("createSocket, connect ioctl FIONBIO 1 failed, ret:%d, fd:%d, path:%s", ret, fd, remotePath);
            return -3;
        }
        ret = connect(fd, (struct sockaddr *) &un, len);
        pwrn("createSocket, connect socket, ret:%d, local:%s, remote:%s", ret, localPath, remotePath);
        if (ret == -1) {
            if (errno == EINPROGRESS) {
                struct pollfd pfd = {0, 0, 0};
                pfd.fd = fd;
                pfd.events = POLLIN;
                if (poll(&pfd, 1, 5000) > 0) {
                    int scklen = sizeof(int);
                    int error;
                    getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&scklen);
                    if (error != 0) {
                        close(fd);
                        perr("createSocket, connect poll getsockopt failed ret:%d fd:%d path:%s", ret, fd, remotePath);
                        return -3;
                    }
                } else { //timeout or poll error
                    close(fd);
                    perr("createSocket, connect poll timeout ret:%d fd:%d path:%s", ret, fd, remotePath);
                    return -3;
                }
            } else {
                close(fd);
                perr("createSocket, connect failed ret:%d fd:%d path:%s", ret, fd, remotePath);
                return -3;
            }
        }

        opt = 0;
        //set blocking
        if (ioctl(fd, FIONBIO, &opt) < 0) {
            close(fd);
            perr("connect ioctl FIONBIO 0 failed ret:%d fd:%d path:%s", ret, fd, remotePath);
            return -3;
        }

        if (ret < 0) {
            close(fd);
            perr("connect ret:%d fd:%d path:%s", ret, fd, remotePath);
            return -3;
        }

        m_fd = fd;
        pwrn("create ClientProtocal socket:%d local:%s remote:%s", m_fd, localPath, remotePath);
        return fd;
    }

    int startLoop(int isServer) {
        return loop(isServer);
    }

    int64_t clientSendData(const uint8_t *data, const int len) {
        return sendData(m_fd, data, len);
    }

    int64_t sendData(const int fd, const uint8_t *data, const int len) {
        if (data == NULL || len <= 0) {
            return -1;
        }
        SendPack *sendPack = new SendPack();
        sendPack->fd = fd;
        sendPack->data = new uint8_t[len];
        memcpy(sendPack->data, data, len);
        sendPack->len = len;
        sendPack->offset = 0;
        m_sendQueue.push(sendPack);
        write(m_pipefd[1], &m_pipefd[1], sizeof(int));
        return 0;
    }

    // find client socket fd
    int findFdByUid(uint32_t uid) {
        pdbg("findFdByUid uid:%d size:%d", uid, static_cast<int>(mapUid.size()));
        if (mapUid.empty()) {
            return -1;
        }

        std::map<int, std::pair<uid_t, std::string> >::iterator iter;
        for(iter = mapUid.begin(); iter!=mapUid.end(); iter++) {
            if (iter->second.first == uid) {
                pdbg("findFdByUid uid:%d found ret:%d", uid, iter->first);
                return iter->first;
            }
        }

        pdbg("findFdByUid uid:%d not found ret -1", uid);
        return -1;
    }
};

#endif //HARDCODER_LOCALSOCKET_H
