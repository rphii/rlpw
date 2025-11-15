#ifndef RLPW_TASK_H

#include <stdbool.h>
#include <sys/types.h>
#include "pw-user.h"

typedef struct Pw Pw;
typedef struct Pw_Sched Pw_Sched;

typedef struct Pw_Task {
    unsigned int id;
    pthread_t thread;
    pthread_mutex_t wait;
    bool initialized;
    Pw *pw;
    Pw_User *user;
} Pw_Task, *Pw_Tasks;

void *pw_task(void *void_pw_task);

#define RLPW_TASK_H
#endif

