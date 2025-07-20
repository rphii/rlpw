#ifndef BGW_TASK_H

#include <stdbool.h>
#include <sys/types.h>
#include "bgw-user.h"

typedef struct Bgw Bgw;
typedef struct Bgw_Sched Bgw_Sched;

typedef struct Bgw_Task {
    uint id;
    pthread_t thread;
    pthread_mutex_t wait;
    bool initialized;
    Bgw *bgw;
    Bgw_User *user;
} Bgw_Task, *Bgw_Tasks;

void *bgw_task(void *void_bgw_task);

#define BGW_TASK_H
#endif

