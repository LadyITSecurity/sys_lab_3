#ifndef SYS_LAB_3_COMMON_H
#define SYS_LAB_3_COMMON_H

#include "check.hpp"
enum actions {
    START,
    GUESS,
    ANSWER,
    END,
    CONTINUE
};

struct message {
    actions action;
    int data;
};


constexpr int PORT = 60001;

inline bool try_send(int fd, const message& msg) {
    errno = 0;
    int size = check_except(send(fd, &msg, sizeof(message), 0), EPIPE);
    return size > 0;
}

inline bool try_recv(int fd, message& msg) {
    errno = 0;
    int size = check_except(recv(fd, &msg, sizeof(message), MSG_WAITALL), EPIPE);
    return size > 0;
}

#endif //SYS_LAB_3_COMMON_H